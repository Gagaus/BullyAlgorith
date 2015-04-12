/*
   Logical clock implementation using IPC. 
  
   Execute:
   $ ./lamport 0 & ./lamport 1 & ./lamport 2 & ./lamport 3 ...

   Output sample:

P1: --(1)--> P4
P1: (2)
P1: --(3)--> P0
P0: <-(3)-- P1
    (4)
P0: --(5)--> P1
P0: --(6)--> P2
P1: --(4)--> P2
P1: (5)
P0: (7)
P0: --(8)--> P3

 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ds.h"

#define MY_MSGTYPE 1
#define N 5

typedef struct msgbuf {
  long mtype;      /* message type */
  int  c;          /* clock */
  int  sender;
} Msgbuf;

Msgbuf inbuf, outbuf;

int queue_id[N];  /* Communication queues */
int pid;          /* Process' id */
int c;            /* Clock do processo */

int main(int argc, char* argv[]) {
  int j, ne, e_type;

  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return 1;
  }

  sscanf(argv[1], "%d", &pid);

  if (pid >= N) {
    printf("Pid %d out of bounds.\n", pid);
    return 1;
  }

  get_queues();

  srand(time(NULL));

  for (ne = 0; ne < 10; ne++)  {
    e_type = rand() % 3;
    switch (e_type) {
    case 0:   /* Internal event */
      c++;  
      printf("P%d: (%d)\n", pid, c);
      break;
    case 1:   /* Send event */
      c++;
      outbuf.mtype = DS_MESSAGE_TYPE;
      outbuf.c = c;
      outbuf.sender = pid;
      while ((j = rand() % N) == pid); /* Receiver id */
      send_message(j, &outbuf, sizeof(Msgbuf));
      printf("P%d: --(%d)--> P%d\n", pid, c, j);
      break;
    default: /* Receive message */
      if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) == 0) {
	  c = c > inbuf.c ? c + 1 :  inbuf.c + 1;
	  printf("P%d: <-(%d)-- P%d\n", pid, inbuf.c, inbuf.sender);
	  printf("    (%d)\n", c);
	}
    }
  }                   
  return 0;
}
