#ifndef INCLUDED_COMM_H
#define INCLUDED_COMM_H
#include <stdint.h> //needed for the uint_X_t typedef

#define COM_SENDVOTE_ERROR -1
#define COM_SENDVOTE_SUCCESS 1
#define COM_SENDVOTE_ALREADYVOTED 2

#define COM_PORT 1091


#define COM_COMMAND_GETPOLITICS 1
#define COM_COMMAND_SENDVOTE 2
#define COM_COMMAND_GETSTATS 3
#define COM_COMMAND_GETPARTIS 4

typedef struct
{
    uint32_t numNational;
    uint16_t politicien;

} vote;

typedef struct
{
int sock;
} Connection;

typedef struct
{
char nom[50];
uint16_t id;
uint16_t parti;
} politicien;

typedef struct
{
	uint16_t id;
	char nom[50];
} parti;

typedef struct
{
	uint8_t myCommand;
} command;


char *commerr;





/**
 * Opens the connection.
 * Returns 0 if connection error.
 */
Connection* openConnection(char* ip, int port);


/**
 * Ferme la connection
 *
 */
void closeConnection(Connection *);


// Methodes Client

/**
 *
 */

/**
 * Envoye le vote
 * -1 pour erreur (time-out, etc...)
 */
int sendVote(Connection*,vote*);


void sendReply(int sock, int8_t reply);


/**
 * Renvoie l'entier de reply.
 * -1 pour erreur (time-out, etc...)
 */
int8_t getReply(Connection*);

// Methodes pour ClientJournaliste
uint32_t* getStatistiques(Connection* myConnect,int nbrPoliticiens);

uint16_t getPolitiques(Connection* myConnect,politicien** listePol);
uint16_t getPartis(Connection* myConnect,parti** listeParti);
#endif
