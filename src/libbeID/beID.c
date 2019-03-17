#include <stdio.h>
#include <stdlib.h>
#include "opensc/opensc.h"
#include "opensc/asn1.h"
#include <string.h>
#include "beID.h"
#include <math.h>
#include "cards.h"
int slot;
sc_card_t *card;
sc_context_t *ctx;
sc_reader_t *reader;

int hasCard()
{
    return sc_detect_card_presence(reader,0);

}

int waitForCardInsert()
{
    unsigned int evt=0;
    int rd = 0;
    int slots = 1;
    sc_wait_for_event(&reader, &slots, 1,
                      SC_EVENT_CARD_INSERTED,
                      &rd, &evt, -1);

                      return 0;
}
int waitForCardRemove()
{
    unsigned int evt=0;
    int rd = 0;
    int slots = 1;
    sc_wait_for_event(&reader, &slots, 1,
                      SC_EVENT_CARD_REMOVED,
                      &rd, &evt, -1);

                      return 0;
}
static int read_transp(sc_card_t *card, const char *pathstring, unsigned char *buf, int buflen)
{
	sc_path_t path;
	int r;

	sc_format_path(pathstring, &path);
	r = sc_select_file(card, &path, NULL);
	if (r < 0)
		fprintf(stderr, "\nFailed to select file %s: %s\n", pathstring, sc_strerror(r));
	else {
		r = sc_read_binary(card, 0, buf, buflen, 0);
		if (r < 0)
			fprintf(stderr, "\nFailed to read %s: %s\n", pathstring, sc_strerror(r));
	}

	return r;
}



beID* do_belpic(sc_card_t *card)
{

	beID *myBEID = malloc(sizeof(beID));
	/* Contents of the ID file (3F00\DF01\4031) */

	struct {
		char cardnumber[12 + 1];
		char chipnumber[2 * 16 + 1];
		char validfrom[10 + 1];
		char validtill[10 + 1];
		char deliveringmunicipality[50 + 1];  /* UTF8 */
		char nationalnumber[12 + 1];
		char name[90 + 1]; /* UTF8 */
		char firstnames[75 + 1]; /* UTF8 */
		char initial[3 + 1]; /* UTF8 */
		char nationality[65 + 1]; /* UTF8 */
		char birthlocation[60 + 1]; /* UTF8 */
		char birthdate[12 + 1];
		char sex[1 + 1];
		char noblecondition[30 + 1]; /* UTF8 */
		char documenttype[5 + 1];
		char specialstatus[5 + 1];
	} id_data;
	int cardnumberlen = sizeof(id_data.cardnumber);
	int chipnumberlen = sizeof(id_data.chipnumber);
	int validfromlen = sizeof(id_data.validfrom);
	int validtilllen = sizeof(id_data.validtill);
	int deliveringmunicipalitylen = sizeof(id_data.deliveringmunicipality);
	int nationalnumberlen = sizeof(id_data.nationalnumber);
	int namelen = sizeof(id_data.name);
	int firstnameslen = sizeof(id_data.firstnames);
	int initiallen = sizeof(id_data.initial);
	int nationalitylen = sizeof(id_data.nationality);
	int birthlocationlen = sizeof(id_data.birthlocation);
	int birthdatelen = sizeof(id_data.birthdate);
	int sexlen = sizeof(id_data.sex);
	int nobleconditionlen = sizeof(id_data.noblecondition);
	int documenttypelen = sizeof(id_data.documenttype);
	int specialstatuslen = sizeof(id_data.specialstatus);

	struct sc_asn1_entry id[] = {
		{"cardnumber", SC_ASN1_UTF8STRING, 1, 0, id_data.cardnumber, &cardnumberlen},
		{"chipnumber", SC_ASN1_OCTET_STRING, 2, 0, id_data.chipnumber, &chipnumberlen},
		{"validfrom", SC_ASN1_UTF8STRING, 3, 0, id_data.validfrom, &validfromlen},
		{"validtill", SC_ASN1_UTF8STRING, 4, 0, id_data.validtill, &validtilllen},
		{"deliveringmunicipality", SC_ASN1_UTF8STRING, 5, 0, id_data.deliveringmunicipality, &deliveringmunicipalitylen},
		{"nationalnumber", SC_ASN1_UTF8STRING, 6, 0, id_data.nationalnumber, &nationalnumberlen},
		{"name", SC_ASN1_UTF8STRING, 7, 0, id_data.name, &namelen},
		{"firstname(s)", SC_ASN1_UTF8STRING, 8, 0, id_data.firstnames, &firstnameslen},
		{"initial", SC_ASN1_UTF8STRING, 9, 0, id_data.initial, &initiallen},
		{"nationality", SC_ASN1_UTF8STRING, 10, 0, id_data.nationality, &nationalitylen},
		{"birthlocation", SC_ASN1_UTF8STRING, 11, 0, id_data.birthlocation, &birthlocationlen},
		{"birthdate", SC_ASN1_UTF8STRING, 12, 0, id_data.birthdate, &birthdatelen},
		{"sex", SC_ASN1_UTF8STRING, 13, 0, id_data.sex, &sexlen},
		{"noblecondition", SC_ASN1_UTF8STRING, 14, 0, id_data.noblecondition, &nobleconditionlen},
		{"documenttype", SC_ASN1_UTF8STRING, 15, 0, id_data.documenttype, &documenttypelen},
		{"specialstatus", SC_ASN1_UTF8STRING, 16, 0, id_data.specialstatus, &specialstatuslen},
		{NULL, 0, 0, 0, NULL, NULL}
	};

	/* Contents of the Address file (3F00\DF01\4033) */
//	struct {
//		char streetandnumber[63 + 1]; /* UTF8 */
//		char zipcode[4 + 1];
//		char municipality[40 + 1]; /* UTF8 */
//	} address_data;
/*	int streetandnumberlen = sizeof(address_data.streetandnumber);
	int zipcodelen = sizeof(address_data.zipcode);
	int municipalitylen = sizeof(address_data.municipality);
	struct sc_asn1_entry address[] = {
		{"streetandnumber", SC_ASN1_UTF8STRING, 1, 0, address_data.streetandnumber, &streetandnumberlen},
		{"zipcode", SC_ASN1_UTF8STRING, 2, 0, address_data.zipcode, &zipcodelen},
		{"municipal", SC_ASN1_UTF8STRING, 3, 0, address_data.municipality, &municipalitylen},
		{NULL, 0, 0, 0, NULL, NULL}};
*/
	unsigned char buff[512];
	int r;

	r = read_transp(card, "3f00df014031", buff, sizeof(buff));
	if (r < 0)
		goto out;

	memset(&id_data, 0, sizeof(id_data));

	r = sc_asn1_decode(card->ctx, id, buff, r, NULL, NULL);
	if (r < 0) {
		fprintf(stderr, "\nFailed to decode the ID file: %s\n", sc_strerror(r));
		goto out;
	}

//	exportprint("BELPIC_CARDNUMBER", id_data.cardnumber);
//	bintohex(id_data.chipnumber, chipnumberlen);
//	exportprint("BELPIC_CHIPNUMBER", id_data.chipnumber);
//	exportprint("BELPIC_VALIDFROM", id_data.validfrom);
//	exportprint("BELPIC_VALIDTILL", id_data.validtill);
//	exportprint("BELPIC_DELIVERINGMUNICIPALITY", id_data.deliveringmunicipality);
//	exportprint("BELPIC_NATIONALNUMBER", id_data.nationalnumber);
////	exportprint("BELPIC_NAME", id_data.name);
//	exportprint("BELPIC_FIRSTNAMES", id_data.firstnames);


//printf("%s\n",id_data.nationalnumber);
//printf("%d\n",strtoul(id_data.nationalnumber,NULL,6));
int i;

uint32_t numeroNational = 0;
for(i = 0;i < 9;i++)
    {
        //printf("%d\n",id_data.nationalnumber[i]);
        int nn = id_data.nationalnumber[i] - 48;
     //   printf("pow : %d * %f\n",nn,pow(10,8-i));
        numeroNational += nn*pow(10,8-i);
       // printf("%d\n",numeroNational);
    }
//myBEID->prenom = id_data.firstnames;

//printf("%s\n",id_data.firstnames);
strcpy(myBEID->prenom,id_data.firstnames);
//printf("1\n");
strcpy(myBEID->nom,id_data.name);
//printf("2\n");
myBEID->numeroNational = numeroNational;

if(id_data.sex[0] == 'M')
myBEID->sexe = BEID_SEXE_MALE;
else
myBEID->sexe = BEID_SEXE_FEMALE;
printf("%s\n",id_data.sex );

//myBEID->nom = id_data.name;
//myBEID->numeroNational = id_data.nationalnumber;

////	exportprint("BELPIC_INITIAL", id_data.initial);
//	exportprint("BELPIC_NATIONALITY", id_data.nationality);
////	exportprint("BELPIC_BIRTHLOCATION", id_data.birthlocation);
//	exportprint("BELPIC_BIRTHDATE", id_data.birthdate);
//	exportprint("BELPIC_SEX", id_data.sex);
//	exportprint("BELPIC_NOBLECONDITION", id_data.noblecondition);
//	exportprint("BELPIC_DOCUMENTTYPE", id_data.documenttype);
//	exportprint("BELPIC_SPECIALSTATUS", id_data.specialstatus);

	//r = read_transp(card, "3f00df014033", buff, sizeof(buff));
	//if (r < 0)
		//goto out;

//	memset(&address_data, 0, sizeof(address_data));

//	r = sc_asn1_decode(card->ctx, address, buff, r, NULL, NULL);
//	if (r < 0) {
//		fprintf(stderr, "\nFailed to decode the Address file: %s\n", sc_strerror(r));
//		goto out;//
	//}

//	exportprint("BELPIC_STREETANDNUMBER", address_data.streetandnumber);
	//exportprint("BELPIC_ZIPCODE", address_data.zipcode);
	//exportprint("BELPIC_MUNICIPALITY", address_data.municipality);

out:
	return myBEID;
}

int main()
{
    //printf("Hello world!\n");





    sc_establish_context(&ctx, "appliVote");
    reader = ctx->reader[0];
    //printf("Connected to %s\n",(reader)->name);

    waitForCardInsert();
    sc_connect_card(reader, 0, &card);

 beID *myBEID = NULL;

   if (card->type == SC_CARD_TYPE_BELPIC_EID)
			{
			    	printf("BelPic card detected\n");
			    	//do_belpic(card);
			    	myBEID = getBeID();
			}
	else {
		printf("Not a Belpic card!\n");

	}

printf("Nom : %s\n",myBEID->nom);
printf("Num National : %d\n",myBEID->numeroNational);


    if(card != NULL)
       sc_disconnect_card(card, 0);

    waitForCardRemove();

    return 0;
}

int beID_init()
{

    sc_establish_context(&ctx, "appliVote");
    if(ctx == NULL)
        return 0;
    reader = ctx->reader[0];

return 1;
}
void beID_finish()
{
     if(card != NULL)
       sc_disconnect_card(card, 0);

    }

beID* getBeID()
{

    beID* myBEID = NULL;

    sc_connect_card(reader, 0, &card);
    if (card->type == SC_CARD_TYPE_BELPIC_EID)
			{

			    	 myBEID = do_belpic(card);
			}




     if(card != NULL)
       sc_disconnect_card(card, 0);



    return myBEID;

}
