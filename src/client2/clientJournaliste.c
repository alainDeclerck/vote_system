#include <stdio.h>
#include <stdlib.h>
#include "comm.h"

politicien* listePol;
parti* listeParti;
uint16_t nbrPol;
uint16_t nbrParti;
uint32_t *myTab;
Connection *myConnect;

uint32_t totalVotes; //Nombre total de votes
uint32_t* votesParti; //Nombre de votes par parti

char* ip;

void loadInfo()
{

	myConnect = openConnection(ip,COM_PORT);
	if(myConnect == NULL)
		{
			printf("Erreur de communication : %s \n",commerr);
			exit(-1);
		}
	//printf("Connected\n");
	nbrParti = getPartis(myConnect,&listeParti);
	//printf("received %d partis",nbrParti);
	closeConnection(myConnect);
	myConnect = openConnection(ip,COM_PORT);
	if(myConnect == NULL)
		{
			printf("Erreur de communication : %s \n",commerr);
			exit(-1);
		}
	nbrPol = getPolitiques(myConnect,&listePol);
	//printf("received %d pols",nbrPol);
	closeConnection(myConnect);
	myConnect = openConnection(ip,COM_PORT);
	if(myConnect == NULL)
		{
			printf("Erreur de communication : %s \n",commerr);
			exit(-1);
		}
	//printf("Asking for stats\n");
	myTab = getStatistiques(myConnect,nbrPol);
	//printf("Got stats\n");
	closeConnection(myConnect);
}

// Calcule le tableau de votes par partis.
void calcStatsPartis()
{

	//Creation du tableau
	votesParti = calloc(nbrParti,sizeof(uint32_t));
	totalVotes= 0;
	// Parcours de la liste de politiciens
	int i=0;
	for(i=0;i<nbrPol;i++)
	{
		politicien* myPol = &listePol[i];
		votesParti[myPol->parti-1] += myTab[i];
		totalVotes+= myTab[i];

	}


}

void showPartis()
{
	int i =0;

	for(i = 0 ; i < nbrParti; i++)
	{
		parti* monParti = &listeParti[i];
		printf("%s\t\%d\t%d%%\n",monParti->nom,votesParti[monParti->id-1],votesParti[monParti->id-1]*100/totalVotes);
	}

}

void showPolitics()
{
	int i =  0;

	while( i < nbrPol)
		{
		printf("%s\t\t%u ) \t\t%d\t%d%%\n",listePol[i].nom,listePol[i].id,myTab[i],myTab[i]*100/totalVotes);
		i++;
		}
}

int main(int argc, char** argv)
{
	ip = "127.0.0.1";
	if(argc >= 2)
		ip = argv[1];

	loadInfo();
	calcStatsPartis();
	showPartis();
	showPolitics();





	return 0;
}
