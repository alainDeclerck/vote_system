#include <stdlib.h>
#include <string.h>
#include "server_politiques.h"

#define _GNU_SOURCE
#include <stdio.h>


/**
 * On renvoye l'id du parti
 * on renvoye -1 si le parti n'est pas trouvé
 */
int getParti(char* parti)
{

	int i;
	int ret=0;
	for(i = 0;i < tailleListePartis;i++)
	{

		ret = strcmp(listePartis[i].nom,parti);

		if(ret == 0)
			return listePartis[i].id;
	}
return -1;
}
/**
 * On ajoute le parti dans le tableau et on renvoye l'id
 */
int addParti(char* parti)
{

	strcpy(listePartis[tailleListePartis].nom,parti);
	tailleListePartis++;
	listePartis[tailleListePartis-1].id = tailleListePartis;
	return tailleListePartis;


}

char* ReadToTab(char **buff, char* s, int size)
{

    int i;
    int inchar;
    for (i = 0; i < size - 1; i++)
    {
        inchar = *(*buff)++;
        if ((inchar == '\t') ||(inchar == '\n'))
        {
            break;
        }
        else
        {
            s[i] = inchar;
        }
    }
    s[i] = '\0';

    return s;
}


int loadPolitiques( politicien** ret)
{
	// lecture du fichier texte

	char *inname = "politiciens.txt";
    FILE *infile;

	// determination du nbr de politiciens
	int nbrPol = 0;

	infile = fopen(inname, "r");
	    if (!infile)
	    {
	        printf("Couldn't open %s for reading\n",inname);
	        return 0;
	    }


	    char buf[255];
	    char *pBuf;
	    int inchar;

	    while (fgets(buf,255,infile))
	    { int i =0;
	    pBuf = buf;
	    	while(i++ < 255)
	    	{
	    		inchar = (*pBuf)++;

	    	        if ((inchar == '\n'))
	    	        {
	    	        	nbrPol++;
	    	        }
	    	}
	    }


printf("Found %d pols\n",nbrPol);

rewind(infile); // On retourne au debut.

int i=0;
ssize_t readsize;
size_t len;
char * line= NULL;
char * pline= NULL;
politicien *myPols = calloc(nbrPol+1,sizeof(politicien));
listePartis = calloc(30,sizeof(listePartis)); // 30 devrait sufir ...
	 while ((readsize = getline(&line, &len, infile)) != -1) {
			 pline = line;

				 ReadToTab(&pline,myPols[i].nom,50);
			//		 printf("Nom : %s\n",myPols[i].nom);
					 char* parti = malloc(50);
					 ReadToTab(&pline,parti,50);
				//	 printf("Parti : %s\n",parti);
					    int idparti = getParti(parti);
					   if(idparti == -1)
					    	idparti = addParti(parti);
		//		    printf("ID Parti : %d\n",idparti);
						myPols[i].parti = idparti;

						myPols[i].id =  i+1;
						i++;

	           if (line)
	           {
	        	   //free(line);
	           line = NULL;
	           }


	}
	*ret = myPols;

return nbrPol;


}
