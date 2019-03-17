#include "dynTable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

unsigned long int maxSize=33554432;

unsigned long int dynTableGetMaxPageSize()
{
	struct shminfo myInfo;

	shmctl(0,3,(void*)&myInfo); // 3 = IPC_INFO

	return myInfo.shmmax;


}

dynTable* dynTableNew(unsigned int elem_size, uint32_t nbrElem, float memIncFactor)
{
	maxSize = dynTableGetMaxPageSize()-1024;

	if(nbrElem == 0)
		nbrElem = 256;
	void *pntr =0;
	int shmid = dynTableRegMem(&pntr,sizeof(dynTable) + (nbrElem*elem_size) +sizeof( dynTableShSegment) );
	dynTable* myTable = (dynTable*) pntr;
	if(myTable == NULL)
		return NULL;
	myTable->elemSize = elem_size;
	myTable->used = 0;
	myTable->size = nbrElem;
	dynTableShSegment *mySeg = (dynTableShSegment*) myTable + sizeof(dynTable);

	//myTable->ShSegmentList = dynTableNewShSegment(nbrElem*elem_size);

	//dynTableShSegment *mySeg = malloc(sizeof( dynTableShSegment)+size);
		mySeg->mempntr = mySeg + sizeof(dynTableShSegment);
		mySeg->memsize = (nbrElem*elem_size);
		mySeg->next = 0;
		mySeg->shmid = shmid;
		mySeg->nextshmid = 0;
		myTable->ShSegmentList = mySeg;

	//printf("Created table of %d elems\n",myTable->size);

return myTable;

}
int dynTableRegMem(void** pntr, int size)
{
//return malloc(size);
int i =0;
	 int shmid = shmget((12+size)%720915, size, 0666 | IPC_CREAT | IPC_EXCL);
	    while (shmid == -1)
	    {i++;
					int id = (43+size+i)%720915;

	        shmid = shmget(id, size, 0666 | IPC_CREAT | IPC_EXCL);
	      //  printf("shmget() of size %d with key %d failed : %d\n",size,id,shmid);
	       // printf("errno : %s\n",strerror(errno));
	    }
	    void* shm_address = shmat(shmid, NULL, 0);
	    if ( shm_address==NULL )
	    {
	        printf("shmat() failed\n");
	        return 0;
	    }
	    *pntr = shm_address;
	//    dynTableFormat(pntr,size);
	    return shmid;

}

void dynTableGrow(dynTable* myTable, uint32_t nbrElem)
{



	if(nbrElem == 0)
		nbrElem = 	myTable->size;
	if(nbrElem == 0)
		if(nbrElem == 1024*1000); // security

	int size = nbrElem * myTable->elemSize;
	if(size >= maxSize)
		size = maxSize-1;

	dynTableShSegment *last = myTable->ShSegmentList;

	while(last->next != NULL)
	{
	//	printf("Sh Info : %d\n",last->memsize);
		last = last->next;
	}
	//printf("Sh Info : %d\n",last->memsize);
	last->next = dynTableNewShSegment(size);
	last->nextshmid = last->next->shmid;
	// Now we update the Table properties

	myTable->size += nbrElem;
	//printf("Growed Table to : %u elems\n",myTable->size);


}

dynTableShSegment* dynTableNewShSegment(uint32_t size)
{
	void* pntr;
	  int shmid = dynTableRegMem(&pntr,sizeof( dynTableShSegment)+size);
	  dynTableShSegment *mySeg = pntr;

	mySeg->mempntr = mySeg + sizeof(dynTableShSegment);
	mySeg->memsize = size;
	mySeg->next = 0;
	mySeg->shmid = shmid;
	mySeg->nextshmid = 0;
	mySeg->dirty=1;

//printf("Created new Segment: %d bytes\n",mySeg->memsize);
	return mySeg;

}

void* dynTableGet(dynTable* myTable,uint32_t index)
{
//printf("Looking in dyTAble for index : %d\n",index);
		if( myTable->used < index)
			return NULL;

//printf("used in dynTAble : %d",myTable->used);
	void* memZone = dynTableIndexToMem(myTable,index);

//printf("Found a memZone, sending \n");
int* test = (int*) memZone;
if(*test == 0)
	return NULL;

return memZone;

}
void dynTableDestroy(dynTable* myTable)
{
	// Lets go thru every Segment and reclaim the shm !!
	dynTableShSegment *mySeg;
	dynTableShSegment *mySeg_t;
	mySeg = myTable->ShSegmentList;

	while( (mySeg !=0) && (mySeg->nextshmid != 0 )  )
				{
							mySeg_t= mySeg;
							mySeg = shmat(mySeg->nextshmid,NULL,0);
							shmctl(mySeg_t->shmid, IPC_RMID, NULL);
							shmdt(mySeg_t);

				}
	shmctl(mySeg->shmid, IPC_RMID, NULL);
shmdt(mySeg);

}
void* dynTableIndexToMem(dynTable *myTable,uint32_t index)
{

	index = index*myTable->elemSize;

	//printf("1\n");

		dynTableShSegment *mySeg = myTable->ShSegmentList;
		unsigned int counter=mySeg->memsize;
		//printf("2\n");
		if(mySeg == 0 )
						return NULL;
			//printf("3\n");
			while( (counter < index) && (mySeg !=0) && (mySeg->next != 0 )  )
			{
			//			printf("8\n");
						mySeg = shmat(mySeg->nextshmid,NULL,0);
		//		mySeg = mySeg->next;
			//		printf("9 : %d\n",counter);
				counter+=mySeg->memsize;
				//		printf("10\n");
			}
		//	printf("4\n");
			if(mySeg == 0 )
				return NULL;
			//		printf("5\n");
		unsigned int absStart = counter - mySeg->memsize;
		unsigned int relStart = index - absStart;

		void* memWrite = relStart + mySeg->mempntr;
		return memWrite;


}
int dynTableAdd(dynTable *myTable,uint32_t index, void *elem)
{
	//printf("Adding elem at index : %u \n",index);
	while( myTable->size <= index)
		dynTableGrow(myTable,0);

	void* memZone = dynTableIndexToMem(myTable,index);

	memcpy(memZone,elem,myTable->elemSize);

	//printf("Added elem at index : %u \n",index);

	if(index > myTable->used)
		myTable->used = index;
	return 1;

}


void dynTableAppend(dynTable *myTable,void *elem)
{

	if(myTable->size - myTable->used < 5) //Trigger for autogrowth
		dynTableGrow(myTable,0);

	// Ou doit on ecrire ?
	unsigned int nextElem = (myTable->used);


	void* memWrite = dynTableIndexToMem(myTable,nextElem);

    memcpy(memWrite,elem,myTable->elemSize);

    myTable->used++;
  //  //printf("Appended elem at index : %d \n",nextElem);

}
