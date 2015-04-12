/* 
   Vector clock implementation using IPC.

   Execute:
   $ ./vc 0 & ./vc 1 & ./vc 2 & ./vc 3 ...

   Output sample:

P1: --[0, 1, 0, 0, 0]--> P3
P1: [0, 2, 0, 0, 0]
P1: [0, 3, 0, 0, 0]
P0: --[1, 0, 0, 0, 0]--> P3
P1: [0, 4, 0, 0, 0]
P1: --[0, 5, 0, 0, 0]--> P0
P0: [2, 0, 0, 0, 0]
P1: [0, 6, 0, 0, 0]
P0: <-[0, 5, 0, 0, 0]-- P1
    [3, 5, 0, 0, 0]
P0: [4, 5, 0, 0, 0]
P0: [5, 5, 0, 0, 0]
P0: --[6, 5, 0, 0, 0]--> P1
P0: [7, 5, 0, 0, 0]

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ds.h"


int pid;    /* Process' id */
int vc[N];  /* Process' vector clock */

typedef struct msgbuf {
  long mtype;      /* message type */
  int  vc[N];          
  int  sender;
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

void print_vclock(int vc[N]) {
  int i;
  printf("[");
  for (i = 0; i < N-1; i++) {
    printf("%d, ", vc[i]);
  }
  printf("%d]", vc[N-1]);
}

void print_internal_event() {
  printf("P%d: ", pid);
  print_vclock(vc);
  printf("\n");
}

void print_send_message(int j) {
  printf("P%d: --", pid);
   print_vclock(vc);
  printf("--> P%d\n", j);
}

void print_receive_message(Msgbuf* inbuf) {
  printf("P%d: <-", pid); 
  print_vclock(inbuf->vc);
  printf("-- P%d\n", inbuf->sender);
  printf ("    ");
  print_vclock(vc);
  printf("\n");
}

int main(int argc, char* argv[]) {
  int i,j, ne, e_type;
  Msgbuf inbuf, outbuf;

  get_pid_from_argv(argc, argv);

  get_queues();

  srand(time(NULL));

  for (ne = 0; ne < 10; ne++)  {
    e_type = rand() % 3;
    switch (e_type) {
    case 0:
      vc[pid]++;  
      print_internal_event();
      break;
    case 1:
      vc[pid]++;
      outbuf.mtype = 1;
      for (i = 0; i < N; i++)
	outbuf.vc[i] = vc[i];
      outbuf.sender = pid;
      while ((j = rand() % N) == pid); 
      send_message(j, &outbuf, sizeof(Msgbuf));
      print_send_message(j);
      break;
    default:
      if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) == 0) {
	for (i = 0; i < N; i++)
	  if (inbuf.vc[i] > vc[i])
	    vc[i] = inbuf.vc[i];
        vc[pid]++;
	print_receive_message(&inbuf);
      }
    }
  }                 
  return 0;
}
