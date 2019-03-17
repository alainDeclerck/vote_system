/*
 * server_shm.h
 *
 *  Created on: Apr 29, 2009
 *      Author: zark
 */

#ifndef SERVER_SHM_H_
#define SERVER_SHM_H_

#include <sys/shm.h>


int shmInit(long key, int permissions,size_t size);
void* shmAssign(int shm_id);
int shmFree(void* adr_shared_memory, int shm_id);

#endif /* SERVER_SHM_H_ */
