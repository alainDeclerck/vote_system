#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>





#include "comm.h"

void closeConnection(Connection* myConnect)
{
	shutdown(myConnect->sock,SHUT_RDWR);
	close(myConnect->sock);//on ferme le socket
	free(myConnect);
}

Connection* openConnection(char* ip, int port)
{

	Connection * myConnect = (Connection * ) malloc(sizeof(Connection));


	struct sockaddr_in *remote;

	     if((myConnect->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){

	    //perror("Impossible de creer le socket");
	    	commerr = "impossible de creer le socket";



	    return NULL;

	  }
	     struct linger set_linger;
	     	    	set_linger.l_onoff = 1;
	     	    	set_linger.l_linger = 0;
	     	    	setsockopt(myConnect->sock, SOL_SOCKET, SO_LINGER, (char *)&set_linger,sizeof(set_linger));


	  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	  remote->sin_family = AF_INET;
	  // Converti un string en byte pour le struct
	  inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	 // Pareil, mais pour le port
	  remote->sin_port = htons(port);


	// Connection au serveur
	  if(connect(myConnect->sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
			 commerr = "impossible de se connecter";
//	    perror("impossible de se connecter");
	    return NULL;
	  //  exit(1);

	  }


	return myConnect;

}

void sendReply(int sock, int8_t reply)
{
	send(sock, &reply,sizeof(int8_t), 0);
}

int8_t getReply(Connection* myConnect)
{
	int8_t reply=99;
	int readsize = 0;
	readsize = read(myConnect->sock, &reply, sizeof(int8_t));

	return reply;

}
uint16_t getPolitiques(Connection* myConnect,politicien** listePol)
{
uint16_t nbrPol = 0;

command *myCommand = malloc(sizeof(command));
myCommand->myCommand = COM_COMMAND_GETPOLITICS;

send(myConnect->sock, myCommand,sizeof(command), 0);

read(myConnect->sock,&nbrPol,sizeof(uint16_t));

politicien * myPol = malloc( sizeof(politicien) * nbrPol);

int totalsize = nbrPol * sizeof(politicien);
int readsize = 0;
while(readsize < totalsize)
	{
	readsize += read(myConnect->sock,myPol+readsize,totalsize-readsize);
//	printf("read %d / %d \n",readsize,total);
	}

*listePol = myPol;

return nbrPol;
}
uint16_t getPartis(Connection* myConnect,parti** listeParti)
{
uint16_t nbrParti = 0;

command *myCommand = malloc(sizeof(command));
myCommand->myCommand = COM_COMMAND_GETPARTIS;

send(myConnect->sock, myCommand,sizeof(command), 0);

read(myConnect->sock,&nbrParti,sizeof(uint16_t));

parti * myPartis = malloc( sizeof(parti) * nbrParti);

int totalsize = nbrParti * sizeof(parti);
int readsize = 0;
while(readsize < totalsize)
	{
	readsize += read(myConnect->sock,myPartis+readsize,totalsize-readsize);
//	printf("read %d / %d \n",readsize,total);
	}

*listeParti = myPartis;

return nbrParti;
}


uint32_t* getStatistiques(Connection* myConnect, int nbrPoliticiens)
{
	command *myCommand = malloc(sizeof(command));
	myCommand->myCommand = COM_COMMAND_GETSTATS;

	send(myConnect->sock, myCommand,sizeof(command), 0);
int readsize=0;
size_t total = nbrPoliticiens * sizeof(uint32_t);
uint32_t *stats = (uint32_t *) malloc(total);
while(readsize < total)
	{readsize += read(myConnect->sock,stats+readsize,total-readsize);
//	printf("read %d / %d \n",readsize,total);
	}



return stats;
}
// Sends a vote over the wire
int sendVote(Connection* myConnect,vote *packet)
{

	int tmpres;
	int sent=0;


command *myCommand = malloc(sizeof(command));
myCommand->myCommand = COM_COMMAND_SENDVOTE;

void *packet2Send = malloc(sizeof(command) + sizeof(vote));
memcpy(packet2Send,myCommand,sizeof(command));
memcpy(packet2Send+sizeof(command),packet,sizeof(vote));


while(sent < sizeof(packet2Send))
 {

    tmpres = send(myConnect->sock, packet2Send+sent,sizeof(command) + sizeof(vote)-sent, 0);

    if(tmpres == -1){

      return COM_SENDVOTE_ERROR;

    }

    sent += tmpres;

  }





return COM_SENDVOTE_SUCCESS;
    }
