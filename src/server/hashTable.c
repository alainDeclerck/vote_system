#include <stdio.h>
#include <stdlib.h>
#include "hashTable.h"

hashTable* hashTableNew(unsigned int elemSize)
{
hashTable *myHash = malloc(sizeof(hashTable));

myHash->myTable = dynTableNew(elemSize,256*1000,0);
if(myHash->myTable == NULL)
	return NULL;

return myHash;

}

void hashTableAdd(hashTable* myHash,uint32_t key,void* elem)
{
uint32_t hash = hashTableHashCode(key);
//printf("hashReceived : %d",hash);
dynTableAdd(myHash->myTable,hash,elem);
//printf(".");

}

void* hashTableGet(hashTable* myHash, uint32_t key)
{
//	printf("hashTableGEt\n");
	uint32_t hash = hashTableHashCode(key);
	//printf("HASh : %d\n",hash);
	return dynTableGet(myHash->myTable,hash);


}

void hashTableDestroy(hashTable *myHash)
{

	dynTableDestroy(myHash->myTable);

}


uint32_t hashTableHashCode(uint32_t key)
{
	if((key > 991231999) || (key < 101001) )
		{
		printf("Received abnormal NumeroNational !");
		return 0;

		}

	// Convertir ex: 821022311 en 30433311
	//printf("key : %d\n",key);
	int y = (int) key/ 10000000;
	key = key - y*10000000;
//	printf("key : %d\n",key);
	int m = (int) (key/ 100000);
	key = key - m*100000;
//	printf("key : %d\n",key);
	int d = (int) (key/ 1000);
	key = key - d*1000;
//	printf("key : %d\n",key);
	int n = (int) key;
/*printf("found y : %d\n",y );
printf("found m : %d\n",m );
printf("found d : %d\n",d );
printf("found n : %d\n",n );
*/
	uint32_t hash = (372 * (y)) + ((m-1)*31) + d;
	hash = hash * 1000 + n;

//printf("hash : %d\n",hash);
	return hash;


}
