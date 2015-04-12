/*
 * Processes send broadcast messages to one another. Messages can be
 * lost or arrive out of order.
 */
#include "ds.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct msgbuf {
  long dstype;     /* IPC message type */
  int  sender;     /* Sender of message */
  char text;       /* Very small message */
} Msgbuf;


int pid;

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
  int l;
  Msgbuf inbuf, outbuf;

  get_pid_from_argv(argc, argv);

  get_queues();

  srand(time(NULL));

  for (l = 0; l < 10; l++)  
    if (rand() % N == 0) {
      outbuf.dstype = DS_MESSAGE_TYPE;
      outbuf.sender = pid;
      outbuf.text = 'A' + rand()%26;
      broadcast(&outbuf, sizeof(outbuf));
      printf("P%d broadcasted %c\n", pid, outbuf.text);
    } else  {
      if (nowait_receive_message(pid, &inbuf, sizeof(inbuf)) == 0)
	printf("P%d received %c\n", pid, inbuf.text);  
    }
  return 0;
}
