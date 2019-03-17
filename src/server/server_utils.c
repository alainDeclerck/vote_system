#include "server_utils.h"
#include "server_messages.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
extern FILE* outputlog;

// display error messages
void error_io_display_msg (char * msg , int num_error) {
  fprintf(outputlog,"I/O ERROR [%s - %d]\n",msg ,num_error);
  exit(-1);
}

// A dotted line
void line() {
	int i=0;
	for(i=0; i < NBR_DOT; i++)
		//if ( write(1,DOT,sizeof(DOT)) == -1 ) error_io_display_msg (WRITE_ON_SCREEN,errno);
		fprintf(outputlog,DOT);
	//for
} // line()

