
#include "server_options.h"
#include "server_messages.h"

#include <stdio.h>
#include <stdlib.h>
extern int is_verbose_mode;
extern int is_save_mode;
extern int is_debug_mode;
extern void line();
// OPTIONS
void options(int argc,char * argv[]) {

	if ( argc == 2 ) {
		if ( argv[1][0] == '-' ) {
			if ( (int)argv[1][2] ) {
				printf(MULTI_OPTIONS);
				exit(-1);
			} else if ( argv[1][1] == 'v' ) {
				// -> Verbose
				printf(VERBOSE);
				is_verbose_mode = 1;
			} else if ( argv[1][1] == 's' ) {
				// -> Save
				printf(SAVE);
				printf("\n(NOT AVAIBLE : under construction)\nnormal mode selected\n\n");
				is_save_mode = 1;
			} else if ( argv[1][1] == 'd' ) {
				// -> Debug
				printf(DEBUG);
				is_debug_mode = 1;
				printf(VERBOSE);
				is_verbose_mode = 1;
			} else {
				printf(INVALID_OPTION);
				exit(-1);
			} // [options]
		// -> Aide
		} else if ( argv[1][0] == '/' && argv[1][1] == '?' ) {

			printf("\n[VOTE SOFTWARE] : \n");
			line();
			printf("\n\n");
			printf("USAGE :\n");
			printf("%s [-v] verbose mode\n",argv[0]);
			printf("%s [-s] save mode (NOT AVAIBLE : under construction) \n",argv[0]);
			printf("%s [-d] debug mode (with verbosing)\n",argv[0]);
			printf("%s [/?] help\n\n",argv[0]);
			exit(-1);

		} // Aide
	} // fi
} // end options
