/*
 *  Distributed system implementation using IPC.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds.h"

int queue_id[N];

/* Get N communication queues. */
void get_queues() {
  int i;
  key_t key;

  for (i = 0, key = QUEUE_KEY_PREFIX; i < N; i++, key++)
    if ((queue_id[i] = msgget(key+i, IPC_CREAT | 0700)) == -1) {
      perror("msgget error");
      exit(2);
    }
}

/* Send a message to Pj */
void send_message(int j, const void* buffer, size_t msgsize) {

  printf("(%ld,%d)\n", ((Msgbuf*)buffer)->mtype, j);

  ((Msgbuf*)buffer)->receiver = j;
  if (msgsnd(queue_id[j], buffer, msgsize, 0) == -1) {
    perror("msgsnd error");
    exit(2);
  }
  if (msgsnd(queue_id[MONITOR_PID], buffer, msgsize, 0) == -1) {
    perror("msgsnd to monitor error");
    exit(2);
  }  
}

/* Broadcast a message to all processes, including itself */
void broadcast(const void* buffer, size_t msgsize) {
  int i;
  long aux= ((Msgbuf*)buffer)->mtype;
  for (i = 0; i < N; i++){
    ((Msgbuf*)buffer)->mtype = aux;
    send_message(i, buffer, msgsize);
  }
}

/* Wait for a new message */ 
void receive_message(int pid, void* buffer, size_t msgsize) {
  if (msgrcv(queue_id[pid], buffer, msgsize, 
	     DS_MESSAGE_TYPE, 0) == -1) {
    perror("msgrcv error");
    exit(2);
  }	     
}

/* Return -1 immediately if no message is available in the queue. 
   Return 0 and a new message in buffer otherwise. */
int nowait_receive_message(int pid, void* buffer, size_t msgsize) {
  if (msgrcv(queue_id[pid], buffer, msgsize, 
	     DS_MESSAGE_TYPE, IPC_NOWAIT) == -1) {
    if (errno == ENOMSG)
      return -1;
    perror("nowait msgrcv error");
    exit(2);
  }
  return 0;
}
