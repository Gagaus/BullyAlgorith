#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ds.h"

int pid;    /* Process' id */
char data[DATASIZE]; /* Process' data */

typedef enum {ELECTION, OK, COORDINATOR, GENERIC} contentMsg;

typedef struct msgbuf {
  long mtype;      /* message type */
  contentMsg c;
} Msgbuf;

void get_pid_from_argv(int argc, char* argv[]) {

  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    exit(1);
  }

  sscanf(argv[1], "%d", &pid);

  if (pid >= N) {
    printf("Pid %d out of bounds.\n", pid);
    exit(1);
  }
}


int main(int argc, char* argv[]) {
  int l,i,j;
  Msgbuf inbuf, outbuf;
  int idLeader;
  get_pid_from_argv(argc, argv);

  get_queues();

  srand(time(NULL));

  while(true){
	int e = rand() % 100;
    switch (e) {
    case 0: // morre
		sleep(TIME_OF_DEATH);
		// zerar queue
		// chamar eleicao
		break;
	case 1: // manda msg pro lider
		break;
	
  }                 
  return 0;
}
