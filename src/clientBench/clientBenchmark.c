#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


#define PORT 1091

#include "comm.h"
#include "clientBenchmark.h"

int msqid;
key_t key = 20;
vote *packet;
int numWorkers = 1;
int numVotes = 10000;
int activeWorkers = 1;
struct timeval start_time, end_time;
char *ip;


void endBenchmark()
{
	// Clean message Queue
	msgctl(msqid, IPC_RMID, NULL);
	//printf("Father can die now ...\n");


exit(0);
}

void handleSIGCHLD() {
	pid_t res;
	while((res =  waitpid(-1,NULL,WNOHANG)) >0 )
		activeWorkers--;
//printf("STill active : %d\n",activeWorkers);
	if(activeWorkers == 0)
	{
		endBenchmark();
	}
}

void slow()
{
	int total_usecs;
		int totalSent = 0;
	gettimeofday(&start_time, (struct timezone*)0); // Timer Start

	Connection *myConnect;

	packet =  malloc(sizeof(vote));

	int i=-1;
	while(i++ < numVotes)
	    {
		//sleep(5);
	    packet->numNational = 911022311 - i;

	    packet->politicien = (i % 136) + 1;

	 // printf("Sending vote %d for %d...\n",packet->numNational, packet->politicien);
	    myConnect = openConnection(ip,PORT);
	    while(myConnect == NULL)
	    	{
	    	myConnect = openConnection(ip,PORT);
	    	printf("%s\n",commerr);
	    	}

	    sendVote(myConnect,packet);
	int rep =    getReply(myConnect);
/*	if(rep == COM_SENDVOTE_SUCCESS)
		printf("Vote OK\n");
	else if(rep == COM_SENDVOTE_ERROR)
		printf("Voting error\n");
	else if(rep == COM_SENDVOTE_ALREADYVOTED)
		printf("Already voted with this id\n");
*/
	totalSent++;
	    closeConnection(myConnect);
	    }
	  gettimeofday(&end_time, (struct timezone*)0);  /* after time */

	    total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 + (end_time.tv_usec-start_time.tv_usec);
	int avgspeed = totalSent/(total_usecs/ 1000000) ;
	       //printf("Total time was %d mSec.\n", total_usecs/1000);
	    printf("Process sent %d votes in %d milliSec = Speed : %d  votes / seconds \n",totalSent,total_usecs/1000,avgspeed);

}


int main(int argc, char** argv)
{
	ip="127.0.0.1";

		if(argc >= 2)
			ip = argv[1];

if(argc >= 3)
	numWorkers = atoi(argv[2]);
if(argc >= 4)
	numVotes = atoi(argv[3]);

	(void) signal(SIGCHLD, handleSIGCHLD); // Child kills
printf("compiled on : %s\n",__TIMESTAMP__);



	 newSystem();
//	 slow();

return EXIT_SUCCESS;
}

/**
This function must run in it's own process (the father process), and keep filling the message queue.
A msgsnd() call will block until the queue has enough space ...
*/
void fillQueue()
{
    // Run by father process...

    // Update fields
    int i=0;
    time_t t;
    srand((unsigned) time(&t));
    message msg;
    msg.mtype = 1;
    packet = malloc(sizeof(packet));
        packet->numNational = 911022311;
        packet->politicien = 1;

    while(i++ < numVotes)
    {
    packet->numNational = 801022311 - i;

  //  packet->politicien = (i % 136) + 1;
    packet->politicien = (rand() % 136)+1;

    memcpy((msg.mtext),packet,sizeof(vote));

    msgsnd(msqid,&msg,sizeof(vote),0);
    }

//printf("Father finished filling Queue !\n");

}
void doWorkers()
{

    // More forks :p
    int i=0;
    while(i++ < numWorkers)
    {
    	activeWorkers++;
        int res = fork();

        if(res == 0 )
        { // i am child
            worker();
            exit(0);
        }
    }


    }
// V2
void newSystem()
{

        msqid = msgget(key, (IPC_CREAT |IPC_EXCL | 0666));
        if (msqid == -1)
        {
            printf("error creating MessageQueue\n");
            return;
        }

        // Lets fork
    int res = fork();

    if(res == 0 ) // i am child
    {
				fillQueue();
    	        //waitpid(res,NULL,0);

    }
    else if(res==-1)
    {
        perror("Error forking ....");
        exit(-1);
    }
    else
       {

    	sleep(1); // Donner du temps a remplir la messageQueue
		doWorkers();
		//waitpid(getpid(),NULL,0);
		while(1)
			pause();
       }



}
void worker()
{
	gettimeofday(&start_time, (struct timezone*)0); // Timer Start
	int total_usecs;
	int totalSent = 0;


     int read_size =1;
    int mypid = (int) getpid();
// read a message from msg queue
vote *myvote;
message mymsg;
Connection *myConnect;
    while(read_size > 0)
    {
     read_size = msgrcv(msqid, &mymsg, sizeof(vote), 1, IPC_NOWAIT);
     if(read_size == 0)
    	 continue;
     myvote = (vote*) &mymsg.mtext;

  //  printf("Process %d - Sending vote %d for %d...\n",mypid,myvote->numNational, myvote->politicien);

    myConnect = openConnection(ip,PORT);
    while(myConnect == NULL)
    	{
    	myConnect = openConnection(ip,PORT);

    	}

    sendVote(myConnect,myvote);
int rep =    getReply(myConnect);

/*if(rep == COM_SENDVOTE_SUCCESS)
	printf("Vote OK\n");
else if(rep == COM_SENDVOTE_ERROR)
	printf("Voting error\n");
else if(rep == COM_SENDVOTE_ALREADYVOTED)
	printf("Already voted with this id\n");
*/
    closeConnection(myConnect);
    totalSent++;



    }
    gettimeofday(&end_time, (struct timezone*)0);  /* after time */

    total_usecs = (end_time.tv_sec-start_time.tv_sec) * 1000000 +
                     (end_time.tv_usec-start_time.tv_usec);
int avg = totalSent/(total_usecs/ 1000000) ;
       //printf("Total time was %d mSec.\n", total_usecs/1000);
    printf("Process sent %d votes in %d milliSec = Speed : %d votes / seconds \n",totalSent,total_usecs/1000, avg);


}
