
#include "server_vote.h"
#include "server_messages.h"
#include "server_utils.h"
#include "server_shm.h"
#include "server_sema.h"
#include "hashTable.h"
#include "comm.h"
#include "server_politiques.h"
#include <stdio.h>
#include <stdlib.h>

extern int is_verbose_mode;
extern int is_debug_mode;
extern FILE* outputlog;

struct mem_partagee *resulats_des_votes;
hashTable *ayantVotes;
int shm_id = -1;
int mutex = -1;

int vote_initialization() {

	 getPoliticiens();

	// INITIALIZATION
	// key = ftok(PATHNAME,'A'); // TODO: pour utiliser ceci il faut connaitre l'emplacement du fichier sur le serveur
	long key = CLE_COMMUNICATION;
	if (key == -1)
		error_io_display_msg ("key creation on server path",errno);

	// mise en concurence de la structure de vote
	mutex = concurence_initialization(key,mutex,PERMISSIONS);

	// Calcul de la taille du tableau necessaire.
	size_t shmemsize = sizeof(uint32_t) + tailleListePoliticiens* sizeof(uint32_t)+sizeof(uint32_t*);  //

    // PLACAGE D'UNE STRUCTURE DE RESULTATS DE VOTES EN MEMOIRE PARTAGEE
    shm_id = shmInit(key,PERMISSIONS,shmemsize);
    resulats_des_votes = (struct mem_partagee*) shmAssign(shm_id);

    memset(resulats_des_votes, 0, shmemsize); // mise a 0 de la memoire.

    resulats_des_votes->resultat_votes = (uint32_t *) &resulats_des_votes->resultat_votes + sizeof(uint32_t*);
    ayantVotes = hashTableNew(sizeof(uint32_t));
    if(ayantVotes == NULL)
    	{printf("error\n"); exit(0);}


	if ( is_debug_mode ) {
    	fprintf(outputlog,"\nid memoire partagee :%d\n",shm_id);
    	fprintf(outputlog,"\nid mutex :%d\n",mutex);
    	fprintf(outputlog,"adresse votes : 0x%x\n",(int)resulats_des_votes);
    	fprintf(outputlog,"[0]mutex 1 value : %d | mutex 2 value : %d \n",semctl (mutex, 0, GETVAL),semctl (mutex, 1, GETVAL) );
    	getchar();
	}

	return mutex;

}

void getPoliticiens()
{
	tailleListePoliticiens = loadPolitiques( &listePoliticiens );
	if(is_verbose_mode) printf("loaded %d Politiciens \n",tailleListePoliticiens);

}

int vote_liberation() {
	if ( !concurence_liberation(mutex) ) printf("Erreur liberation sema");
	if ( !shmFree(resulats_des_votes,shm_id) ) printf("Erreur liberation shm");
	hashTableDestroy(ayantVotes);
	return 1;
}

// PRIVATE
int a_deja_vote(uint32_t numNational) {

	uint32_t *myNum = hashTableGet(ayantVotes,numNational);

	if(myNum == NULL)
		return 0;
	else if(*myNum == numNational )
			{
			if (is_verbose_mode) printf("%d %s \n",numNational,DEJA_VOTE);
			else printf(NOK);


			return 1;
			}



	return 0;
}

// PRIVATE
int enregistrer_vote(vote* packet) {
	resulats_des_votes->resultat_votes[packet->politicien-1]=resulats_des_votes->resultat_votes[packet->politicien-1]+1;
	//printf("%u votes pour %u \n",resulats_des_votes->resultat_votes[packet->politicien-1],packet->politicien);
	resulats_des_votes->total_votes++;
	uint32_t numNational = packet->numNational;
	hashTableAdd(ayantVotes,numNational,&numNational);

	return 1;
}

int voter(vote* packet, int process_id) {
	int retVal = COM_SENDVOTE_ALREADYVOTED;
	p_redacteur(mutex);

	if (!is_verbose_mode) printf("%s%d ",RECORD_VOTE_N,resulats_des_votes->total_votes);

	if ( !a_deja_vote(packet->numNational) ) {
		// enregistrement du vote et du votant
		// while(!enregistrer_vote(packet)); ^^
		enregistrer_vote(packet);
		retVal = COM_SENDVOTE_SUCCESS;
		if (is_verbose_mode) {
			printf("\n");
			line();
			if (is_debug_mode) printf("\nprocess id : (%d)\n",process_id);
			printf("National num : %d\n",packet->numNational);
			printf("For %s %d\n",POLITICIEN_N,packet->politicien);
			line();
		} else printf(OK);
	} // a_deja_vote()

    v_redacteur(mutex) ;

    return retVal;
}


uint32_t *lire_etats_des_votes_actuels( ) {



	uint32_t *myTab = malloc(tailleListePoliticiens * sizeof(uint32_t));

	// working
		if ( !p_lecteur(mutex) ) {
			printf("\nINTERNAL ERROR : ACCES REFUSE EN LECTURE AUX VOTES\n");
			return 0;
		}

	// Copie du tableau des votes de la memoire partagée vers une memoire propre au process.
	memcpy(myTab,resulats_des_votes->resultat_votes, tailleListePoliticiens * sizeof(uint32_t));



	if ( !v_lecteur(mutex) )
			printf("\nINTERNAL ERROR : ACCES EN LECTURE AUX VOTES : PORTE OUVERTE\n");

	return myTab;

}

void affiche_resultats_des_votes() {
	int i = 0;
	printf(VOTES_RESULTS);
	while(i++ < tailleListePoliticiens)
		printf("\n%s %d : %u",POLITICIEN_N,i,resulats_des_votes->resultat_votes[i-1]);
	printf("\n");
	line();
	printf("[%u votes]\n\n",resulats_des_votes->total_votes);


}
