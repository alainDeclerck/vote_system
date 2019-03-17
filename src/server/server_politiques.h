/*
 * server_politiques.h
 *
 *  Created on: May 6, 2009
 *      Author: zark
 */

#ifndef SERVER_POLITIQUES_H_
#define SERVER_POLITIQUES_H_
#include "comm.h"
#define SIZE 50
politicien* listePoliticiens;
uint16_t tailleListePoliticiens;
parti* listePartis;
int tailleListePartis;

int loadPolitiques( politicien** ret);
//int getPolitiques( politicien** ret);
#endif /* SERVER_POLITIQUES_H_ */
