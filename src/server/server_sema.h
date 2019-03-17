#ifndef SERVER_SEMA_H_
#define SERVER_SEMA_H_

#include <sys/sem.h>
#include <errno.h>



#define NBR_SEM 2 // nombre de semaphores {drapeau,tour}

int concurence_initialization(long key, int mutex, int permissions);
int concurence_liberation(int mutex);
int p_redacteur(int mutex);
int v_redacteur(int mutex);
int p_lecteur(int mutex);
int v_lecteur(int mutex);


#endif /* SERVER_SEMA_H_ */
