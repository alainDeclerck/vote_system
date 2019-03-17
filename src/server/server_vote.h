#ifndef SERVER_VOTE_H_
#define SERVER_VOTE_H_

#include <errno.h>
#include <string.h>
#include "comm.h"

#include "server_shm.h"
#include "server_sema.h"
#include "server_messages.h"
#include "server_options.h"

// INITIALIZATION
#define PERMISSIONS 0600
#define TMP_NBR_VOTANTS 100
#define CLE_COMMUNICATION 5

struct mem_partagee {
	uint32_t total_votes;
	uint32_t *resultat_votes;
	//unsigned int ayant_votes[TMP_NBR_VOTANTS];
};

// FONCTIONS
int vote_initialization();
int vote_liberation();
int a_deja_vote(unsigned int numNational); // private
int enregistrer_vote(vote* packet); // private
int voter(vote* packet, int process_id);
uint32_t*  lire_etats_des_votes_actuels();
void affiche_resultats_des_votes();
void getPoliticiens();

#endif /* SERVER_VOTE_H_ */
