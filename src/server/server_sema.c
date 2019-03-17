
#include "server_sema.h"
#include "server_utils.h"
#include "server_messages.h"

short sarray[NBR_SEM] = {0,0}; // initialization des portes
struct sembuf sema_operations[NBR_SEM];

int concurence_initialization(long key, int mutex, int permissions) {
	// CREATION DU SAS VERS LA SHARED MEMORY
	// semget( semkey, 2, 0666 | IPC_CREAT | IPC_EXCL ); <>?
	mutex = semget(key,NBR_SEM,IPC_CREAT | permissions);
	if (mutex == -1) error_io_display_msg ("creation of airlock before shared memory",errno);

	// INITIALIZATION DES STRUCTURES
	if (semctl(mutex,0,SETALL,sarray) == -1) error_io_display_msg ("semaphores structure initialization",errno);

	// POSITIONNEMENT DES SEMAPHORES
	sema_operations[0].sem_flg = SEM_UNDO;
	sema_operations[1].sem_flg = SEM_UNDO;
	sema_operations[0].sem_num = 0;
	sema_operations[0].sem_op = 1; // ouvre la porte
	sema_operations[1].sem_num = 1;
	sema_operations[1].sem_op = 0; // ini le post-it a zero
	if ( (semop (mutex , sema_operations , 2)) < 0) error_io_display_msg (WRITE_ON_SCREEN,errno);

	return mutex;
}

int concurence_liberation(int mutex) {
	if ( (semctl( mutex, NBR_SEM, IPC_RMID )) < 0 ) return 0;
	return 1;
}

int p_redacteur(int mutex) {

	int si_ok = -2;

	sema_operations[0].sem_num = 0;
  	sema_operations[0].sem_op = -1;
  	si_ok = semop (mutex , sema_operations , 1);

 	if (si_ok >= 0) return 1;
	return 0;

} //p (proberen : essayer)

int v_redacteur(int mutex) {

	int si_ok = -2;

	if ( !semctl(mutex, 1, GETVAL) ) {
		sema_operations[0].sem_num = 0;
		sema_operations[0].sem_op = 1; // ouvre la porte
		sema_operations[1].sem_num = 1;
		sema_operations[1].sem_op = 1; // met le post-it a 1
		si_ok = semop (mutex , sema_operations , 2);
	} else {
		sema_operations[0].sem_num = 0;
		sema_operations[0].sem_op = 1; // ouvre la porte
		si_ok = semop (mutex , sema_operations , 1);
	}

	if (si_ok >= 0) return 1;
	return 0;

} //v (verhogen : augmenter)

int p_lecteur(int mutex) {

	int si_ok = -2;

	sema_operations[0].sem_num = 0;
	sema_operations[0].sem_op = -1; // ferme la porte
	si_ok = semop (mutex , sema_operations , 1);

 	if (si_ok >= 0) return 1;
	return 0;

} //p (proberen : essayer)

int v_lecteur(int mutex) {

	int si_ok = -2;

	sema_operations[0].sem_num = 0;
	sema_operations[0].sem_op = 1; // ouvre la porte
	si_ok = semop (mutex , sema_operations , 1);

	if (si_ok >= 0) return 1;
	return 0;

} //v (verhogen : augmenter)


