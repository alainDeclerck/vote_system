#ifndef INCLUDED_HASHTABLE_H
#define INCLUDED_HASHTABLE_H
#include <stdint.h> //needed for the uint_X_t typedef
#include "dynTable.h"


typedef struct
{

	dynTable *myTable;


} hashTable;


hashTable* hashTableNew(unsigned int);

void hashTableAdd(hashTable*,uint32_t key,void*);

void* hashTableGet(hashTable*, uint32_t key);

uint32_t hashTableHashCode(uint32_t key);

void hashTableDestroy(hashTable *myHash);

#endif
