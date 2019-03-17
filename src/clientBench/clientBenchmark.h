#ifndef INCLUDED_CLIBENCH_H
#define INCLUDED_CLIBENCH_H





typedef struct mymsg {
      long      mtype;    /* message type */
      unsigned int mtext[2]; /* message text of length MSGSZ */
} message;



void worker();
void doWorkers();
void newSystem();
void fillQueue();



#endif
