#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ds.h"

#define TIMEOUT 10
#define SLEEP_TIME 1
#define TIME_OF_DEATH 40
#define NUM_PROCESS 6

long pid;    /* Process' id */
char data[DATASIZE]; /* Process' data */
int leader = 6; // guarda o lider do ds


void get_pid_from_argv(int argc, char* argv[]) {

  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    exit(1);
  }

  sscanf(argv[1], "%ld", &pid);

  if (pid >= N) {
    printf("Pid %ld out of bounds.\n", pid);
    exit(1);
  }
}


typedef struct msgbuf {
  long mtype;      /* message type */
  contentMsg c;
  int receiver;
} Msgbuf;


int main(int argc, char* argv[]) {
	get_pid_from_argv(argc, argv);
	Msgbuf inbuf, outbuf;
	outbuf.mtype = pid;
	
	get_queues();
	
	srand(time(NULL));
	
	while(1){
		msgbuf.mtype = rand()%6 + 1;
		msgbuf.c = rand()%6;
	
		send_message(, msgbuf, size_t msgsize);

		sleep(2000);
	}                 
	return 0;
	}
	
