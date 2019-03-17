#ifndef INCLUDED_DYNTABLE_H
#define INCLUDED_DYNTABLE_H

#define DYNTABLE_INC_LINEAR 1
#define DYNTABLE_INC_EXP 2
#include <stdint.h> //needed for the uint_X_t typedef




typedef struct t_dynTableShSegment
{
void* mempntr;
int shmid;
int nextshmid;
unsigned short int dirty;
unsigned int memsize;

struct t_dynTableShSegment* next;

} dynTableShSegment;





typedef struct {

uint32_t size; // nbr of usable Elements
uint32_t used; // nbr of used Elements
//void* mempntr; // points to start of memory
float incFactor; // Growth Factor
unsigned int elemSize; // size of one element.

dynTableShSegment* ShSegmentList;



} dynTable;


/**
* Must specify the size of one element.
* Can specify an initialSize for the sharedMemory. 0 for default.
*/
dynTable* dynTableNew(unsigned int elem_size, uint32_t memInitialSize, float memIncFactor);
void dynTableDestroy(dynTable*);

/**
* Adds an element to our table
*/
void dynTableAppend(dynTable*,void* elem);
/**
* Adds an element to our table
*/
int dynTableAdd(dynTable*,uint32_t,void* elem);
/**
* Returns the element from our table.
* Be sure to cast !
*/
void* dynTableGet(dynTable*,uint32_t);



// --- PRIVATE FUNCTIONS

/**
* Creates a new ShMem segment and adds it to the list
*/
void dynTableGrow(dynTable*,uint32_t);
int dynTableRegMem(void**,int);

dynTableShSegment* dynTableNewShSegment(uint32_t size);
void* dynTableIndexToMem(dynTable *myTable,uint32_t index);
unsigned long int dynTableGetMaxPageSize();

#endif
