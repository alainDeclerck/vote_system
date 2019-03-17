
#include "server_shm.h"
#include "server_utils.h"
#include "server_messages.h"
#include <errno.h>
#include <stdlib.h>

struct shmid_ds shmid_struct;

int shmInit(long key, int permissions,size_t size) {
	int shm_id = -1;

	// shmget(shmkey, 256, 0666 | IPC_CREAT | IPC_EXCL); <>?
	shm_id = shmget(key, size, IPC_CREAT | permissions);
	if (shm_id == -1) error_io_display_msg ("shared memory creation",errno);

	return shm_id;
}

void* shmAssign(int shm_id) {

	void * adr_shared_memory;

	// ATTACHEMENT DU NOUVEAU SHARED MEMORY A L'ESPACE D'ADRESSE DU PROCESS
	adr_shared_memory = shmat(shm_id,NULL,0); // NULL = creation si pas existant, 0 veut dire read & write
	if ((int)adr_shared_memory == -1) error_io_display_msg("shared memory initialization",errno);


	return adr_shared_memory;

}

int shmFree(void* adr_shared_memory, int shm_id) {
	if ( shmdt(adr_shared_memory) < 0 ) return 0;
	if ( shmctl(shm_id, IPC_RMID, &shmid_struct) < 0 ) return 0;
	return 1;
}

