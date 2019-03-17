#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <stdint.h>
#include "comm.h"
#include "beID.h"
#include <unistd.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4



//Globals
beID *myBEID;
politicien* listePol;
parti* listeParti;
uint16_t nbrPol;
uint16_t nbrParti;
Connection *myConnect;

char* ip;


void processVote(uint16_t politicienID)
{

vote myVote;
myVote.numNational = myBEID->numeroNational;
myVote.politicien = politicienID;
Connection *myConnect;
move(20, 0);
clrtoeol();
		mvprintw(20, 0, "Connection en cours");
myConnect = openConnection(ip,COM_PORT);
while(!myConnect)
	{
	mvprintw(20, 0, "Probleme de connection avec le serveur");
	sleep(5);
	}

move(20, 0);
clrtoeol();
mvprintw(20, 0, "Connection Etablie. Envoie du vote");
int ret = sendVote(myConnect,&myVote);



if(ret == COM_SENDVOTE_SUCCESS)
{
 // Le vote a été envoyé, on peut maintenant tester la valeur de retour !
	ret = getReply(myConnect);
	if(ret == COM_SENDVOTE_ALREADYVOTED)
	{
		move(20, 0);
		clrtoeol();
		mvprintw(20, 0, "Vous avez deja voté !");
	}
	else
	{
		// Vote OK
		// On peux passer a la liste suivante
			move(20, 0);
			clrtoeol();
			mvprintw(20, 0, "Vote ok ! ");

	}




	//return 0;

}
else if(ret == COM_SENDVOTE_ERROR)
{
// Error Sending Vote
	// Mise en place d'un retry ?
	move(20, 0);
	clrtoeol();
	mvprintw(20, 0, "Erreure lors de l'envoie du vote");

	// RETRY


	// Expiration finale
	//return -1;
}

}
int menu();

void displayWelcomeScreen(beID *myBEID)
{
erase();

if(myBEID->sexe == BEID_SEXE_MALE)
	mvprintw(LINES/2-1, 10, "Bonjour Mr %s %s",myBEID->prenom, myBEID->nom);
else
	mvprintw(LINES/2-1, 10, "Bonjour Ms %s %s",myBEID->prenom, myBEID->nom);

refresh();
}

void loadData()
{

				myConnect = openConnection(ip,COM_PORT);
				if(myConnect == NULL)
				{
					printf("Erreur de communication : %s \n",commerr);
					exit(-1);
				}
				printf("Connected\n");
				nbrParti = getPartis(myConnect,&listeParti);
				printf("received %d partis",nbrParti);
				closeConnection(myConnect);
				myConnect = openConnection(ip,COM_PORT);
				nbrPol = getPolitiques(myConnect,&listePol);
				printf("received %d pols",nbrPol);
				closeConnection(myConnect);


}

int showPartis()
{
	int idparti=-1;
	erase();
		initscr();
		ITEM **my_items;
		int c;
		MENU *my_menu;
	        int n_choices, i;


			/* Initialize items */
	        n_choices = nbrParti;
	        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
	        for(i = 0; i < n_choices; ++i)
		{
	        	parti * myParti = &listeParti[i];
	        	my_items[i] =  new_item(myParti->nom, myParti->nom);
			/* Set the user pointer */
			set_item_userptr(my_items[i], (void*) &listeParti[i]);
		}
		my_items[n_choices] = (ITEM *)NULL; // Fin du menu

		/* Create menu */
		my_menu = new_menu((ITEM **)my_items);
		menu_opts_off(my_menu, O_SHOWDESC);

		/* Post the menu */
		mvprintw(LINES - 3, 0, "Appuyez sur <ENTER> pour choisir un parti");
	//	mvprintw(LINES - 2, 0, "Up and Down arrow keys to navigate (ESC to Exit)");

		post_menu(my_menu);
		refresh();
int menuclose=0;
		while( (menuclose==0) && ( (c = getch()) != 27 )  ) /* ESC */
		{       switch(c)
		        {	case KEY_DOWN:
					menu_driver(my_menu, REQ_DOWN_ITEM);
					break;
				case KEY_UP:
					menu_driver(my_menu, REQ_UP_ITEM);
					break;
				case 10: /* Enter */
				{	ITEM *cur;


					cur = current_item(my_menu);
					parti * myParti = item_userptr(cur);
					 idparti = myParti->id;
					//processVote(vote);
					 menuclose = 1;


			//		pos_menu_cursor(my_menu);
					break;
				}
				break;
			}
		}
		unpost_menu(my_menu);
		for(i = 0; i < n_choices; ++i)
			free_item(my_items[i]);
		free_menu(my_menu);

return idparti;
}
int showPoliticiens(int idparti)
{
	int ret = 0;
	erase();
		initscr();
		ITEM **my_items;
		int c;
		MENU *my_menu;
	        int n_choices, i;

	        // Creation d'une array contenant les politiciens du parti I
	        // D'abords on scan pour trouver le nombre de politiciens du parti idparti
n_choices=0;
	        for(i = 0; i < nbrPol;i++)
	        {
	        	politicien *p = &listePol[i];

	        	if(p->parti == idparti)
	        		{
	        		n_choices++;
	        		}
	        }


			/* Initialize items */
int j=0;
	        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
	        for(i = 0; i < nbrPol;i++)
		{
	        	politicien *p = &listePol[i];
	        	if(p->parti == idparti)
	        	{
	        		my_items[j] =  new_item(p->nom, p->nom);
	        		set_item_userptr(my_items[j], (void*) p);
	        		j++;
	        	}
		}
		my_items[n_choices] = (ITEM *)NULL; // Fin du menu

		/* Create menu */
		my_menu = new_menu((ITEM **)my_items);
		menu_opts_off(my_menu, O_SHOWDESC);

		/* Post the menu */
		mvprintw(LINES - 3, 0, "Appuyez sur <ENTER> pour voter");
		mvprintw(LINES - 2, 0, "(ESC pour revenir a la liste des partis)");

		post_menu(my_menu);
		refresh();

		while((ret == 0) && (c = getch()) != 27) /* ESC */
		{       switch(c)
		        {	case KEY_DOWN:
					menu_driver(my_menu, REQ_DOWN_ITEM);
					break;
				case KEY_UP:
					menu_driver(my_menu, REQ_UP_ITEM);
					break;
				case 10: /* Enter */
				{	ITEM *cur;


					cur = current_item(my_menu);
					politicien * myPol = item_userptr(cur);
					int vote = myPol->id;

					ret = vote;
				//	pos_menu_cursor(my_menu);
					break;
				}
				break;
			}
		}
		unpost_menu(my_menu);
		for(i = 0; i < n_choices; ++i)
			free_item(my_items[i]);
		free_menu(my_menu);

return ret;
}

int main(int argc, char* argv[])
{
	ip="127.0.0.1";
	if(argc < 2)
		{
		printf("Vous devez specifier l'adresse ip du serveur\n");
		exit(-1);
		}
	ip = argv[1];


	loadData(); // Chargement des données

	int ret = beID_init(); // Initialisation du lecteur de carte.
	if(ret == 0)
	{
		printf("\nLecteur de carte non trouvé ! \n");
		return(-1);
	}


	// Start Loop
while(1)
{
	initscr();
		start_color();
	        cbreak();
	        noecho();
		keypad(stdscr, TRUE);
		erase();
	/*	init_pair(1, COLOR_RED, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
		*/
		mvprintw(LINES/2-1, COLS/2-9, "Inserez votre carte ...");
		refresh();


		waitForCardInsert();




myBEID = getBeID(); //Lecture info carte

displayWelcomeScreen(myBEID); // affichage info carte

sleep(5); // convivialité

int idvote=0;
int idparti = 0;
while(idvote == 0) // Tant qu'on a pas un id de politicien valable.
{
	idparti = showPartis();
	if(idparti > 0)
		idvote = showPoliticiens(idparti);
}
processVote(idvote);
myBEID = NULL;
mvprintw(LINES/2-1, COLS/2-9, "Veuillez retirer votre carte ...");
		refresh();
waitForCardRemove(); //Fonction bloquante
mvprintw(LINES/2-1, COLS/2-9, "Merci d'avoir utilisé BelVote 3000.");
		refresh();
sleep(5);
}

beID_finish(); // on arrive jamais ici.
endwin();
return 0;
}
