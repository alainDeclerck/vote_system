#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>



#include "server_options.h"
#include "server_vote.h"
#include "server_utils.h"
#include "server_messages.h"
#include "comm.h"
#include "server_politiques.h"
FILE* outputlog;

unsigned short int is_verbose_mode;
unsigned short int is_debug_mode;
unsigned short int is_save_mode;

int sock, newsock;

int queueSize = 128;

void handle(int clientsock, int process_id) {
	int readsize=0;

	command *myCommand = malloc(sizeof(command));

	readsize = read(clientsock,myCommand,sizeof(command));
//	printf("read : %d\n",readsize);
	// Now we know what to do ...
	if(is_verbose_mode) printf("Received command : %d\n",myCommand->myCommand);


	if(myCommand->myCommand == COM_COMMAND_SENDVOTE)
	{
	vote *packet =  malloc(sizeof(vote));
	readsize = read(clientsock, packet, sizeof(vote));
	//printf("read : %d\n",readsize);
	int8_t returnvalue = voter(packet,process_id); // Ecrire en memoire partage
	send(clientsock, &returnvalue,sizeof(int8_t), 0);
	}
	else
		if(myCommand->myCommand == COM_COMMAND_GETSTATS)
			{
		if(is_verbose_mode)	printf("sending stats\n");
			uint32_t *myTab = lire_etats_des_votes_actuels();
			if(myTab == 0) printf("ERROR GETTING VOTES\n");
		//	printf("Tab : %d",*myTab);
			size_t sendSize = (tailleListePoliticiens) * sizeof(uint32_t);
			int s = 0;
			while(s < sendSize)
			s +=	send(clientsock, myTab+s,sendSize-s,0);
			printf("Sent %d / %d\n",s,sendSize);

			}
		else
				if(myCommand->myCommand == COM_COMMAND_GETPOLITICS)
					{

					if(is_verbose_mode) printf("sendin politiciens list\n");
					int sendSize = sizeof(politicien) * tailleListePoliticiens  + sizeof(uint16_t);
					void* packet = malloc( sendSize);
					memcpy(packet,&tailleListePoliticiens,sizeof(uint16_t));
					memcpy(packet+sizeof(uint16_t),listePoliticiens,tailleListePoliticiens*sizeof(politicien));
					int s = 0;
					while(s < sendSize)
						s +=	send(clientsock, packet+s,sendSize-s,0);

					}
			else
					if(myCommand->myCommand == COM_COMMAND_GETPARTIS)
						{

						if(is_verbose_mode) printf("sendin partis list\n");
						int sendSize = sizeof(parti) * tailleListePartis  + sizeof(uint16_t);
						void* packet = malloc( sendSize);
						memcpy(packet,&tailleListePartis,sizeof(uint16_t));
						memcpy(packet+sizeof(uint16_t),listePartis,tailleListePartis*sizeof(parti));
						int s = 0;
						while(s < sendSize)
							s +=	send(clientsock, packet+s,sendSize-s,0);

						}


}

// fermeture propre du serveur
void shutdownServer() {
	line();
	fprintf(outputlog,"%s\n",ENDING_SERVER);
	if ( !vote_liberation() ) fprintf(outputlog,"\n\nPROBLEME DE LIBERATION DE MEMOIRE\n\n");
	close(sock); // close father socket
	exit(0);
}

// Ctrl-C
void handleSIGINT() {
	(void) signal(SIGINT, SIG_DFL); // Now we ignore further calls to Ctrl-C
	affiche_resultats_des_votes();
	shutdownServer();

}

// Avoids zombie processses
void handleSIGCHLD() {
	pid_t res;
	while((res =  waitpid(-1,NULL,WNOHANG)) >0 )
		if (is_verbose_mode)
			fprintf(outputlog,"\n%s (%d)\n",RIP,res );
}

int main(int argc, char * argv[]) {

printf("Version : 0.1a compilation: %s\n",__TIMESTAMP__);

	// INITIALIZATION
	int identity = -1;
	outputlog = stdout;
	// TEST DES AGRUMENTS
	options(argc,argv);

    // Establish signal handlers
    (void) signal(SIGINT, handleSIGINT); //Ctrl-C
    (void) signal(SIGCHLD, handleSIGCHLD); // Child kills


	// init shmem

    struct sockaddr_in server;
    unsigned int addrsize = sizeof (server);//16


    if (is_verbose_mode)
    	fprintf(outputlog,INI_MEM);


    if (!vote_initialization())
    	return -1;

    if (is_verbose_mode)
    	fprintf(outputlog,OK);


	// SOCKET CREATION
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      fprintf(outputlog,"Can't create socket: %s\n", strerror(errno));
      shutdownServer();
    }

    /* Enable address reuse */
    int on = 1;
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

    server.sin_family = AF_INET;
    server.sin_port = htons((short)COM_PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)(&server), addrsize) == -1) {
		fprintf(outputlog,"Can't bind address to port.\n");
		shutdownServer();
    }

	listen(sock, queueSize);//allow x pending requests in queue

	if (!is_verbose_mode)
		fprintf(outputlog,READY);


	while (1) {

		if (is_verbose_mode)
			fprintf(outputlog,CONNECTION);


		if ((newsock = accept(sock, (struct sockaddr *)(&server), &addrsize)) == -1) {
			// ERROR RECEVING CONNECTION
			if (is_verbose_mode)
				fprintf(outputlog,NOK);
			close(newsock);
			newsock = -1;
		} else {
			// WORK WITH NEW PROCESS
			identity = -1;
			if (is_verbose_mode)
				fprintf(outputlog,OK);

			if( (identity = fork()) ) {
				// FATHER PROCESS
				if (is_verbose_mode) {
					fprintf(outputlog,"%s (%d) \n",CHILD_SPAWNED, identity);
				}

				close(newsock); // Lets be clean
			} else {
				// CHILD PROCESS
				identity = getpid();
				close(sock); // close father socket
				 (void) signal(SIGINT, SIG_IGN); //on ignore le CTRL-C pour l'enfant
				handle(newsock,identity);
				// Finished - lets close connection
				shutdown(newsock,SHUT_RDWR);
				close(newsock);

				exit(0); // All done, lets go.

			}

		}

	} // end server loop

} // main
