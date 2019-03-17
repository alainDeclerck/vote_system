
#ifndef INCLUDED_BEID_H
#define INCLUDED_BEID_H


#include "stdint.h"

#define BEID_SEXE_MALE 0
#define BEID_SEXE_FEMALE 1

typedef struct  {

uint32_t numeroNational;
char nom[75 + 1];
char prenom [90 + 1];
uint8_t sexe;

} beID;

int beID_init();
int hasCard();
void beID_finish();
/**
 * Fonction bloquante.
 */
int waitForCardInsert();
int waitForCardRemove();

/**
 * Renvoye une structure BeID
 */
beID* getBeID();

#endif
