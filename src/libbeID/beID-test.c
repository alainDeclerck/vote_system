#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beID.h"
#include <unistd.h>
int slot;


int hasCard()
{
    return 1;

}

int waitForCardInsert()
{
    sleep(5);

                      return 0;
}
int waitForCardRemove()
{
    sleep(5);
                      return 0;
}


int main()
{
return 0;
}

void beID_init()
{




}
void beID_finish()
{

    }

beID* getBeID()
{

    beID *myBeID = (beID*) malloc(sizeof(beID));

strcpy(myBeID->nom,"Jojo");
strcpy(myBeID->prenom, "Le Grand");
myBeID->numeroNational = 821022311;
myBeID->sexe = BEID_SEXE_MALE;

return myBeID;
}
