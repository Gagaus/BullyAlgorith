/*
 * Distributed system implementation using IPC.
 * Monitor implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "process.h"
#include "ds-monitor.h"
#include "print-vector.h"

int queue_id[N];
Monitorbuf q[N];

int vc[N];

void get_monitor_queues() {
 int i;
 key_t key;

 for (i = 0, key = MONITOR_KEY_PREFIX; i < N; i++, key++)
    if ((queue_id[i] = msgget(key, IPC_CREAT | 0700)) == -1) {
      perror("msgget error");
      exit(2);
    }
}

void receive_messages() {
  int i;
    for (i = 0; i < N; i++) {
      if (q[i].etype == NONE && 
	  msgrcv(queue_id[i], &q[i], sizeof(Monitorbuf), 
		 MONITOR_MESSAGE_TYPE, IPC_NOWAIT) == -1 
	  && errno != ENOMSG) {
	perror("nowait msgrcv error");
	exit(2);
      }
    }
}

/* Return 1 if q[i] can be the next event. 
   Return 0 otherwise. */
int next_event(int i) {
  int j;

  if (q[i].etype == NONE) return 0;

  if (q[i].vc[i] > vc[i] + 1) {
    printf("Monitor error: a message has been lost \
or arrived out of order. Sorry!\n");
    exit(1);
  }
  for (j=0; j < N; j++)
    if (j!= i && q[i].vc[j] > vc[j])
      return 0;
  return 1;
}

void monitor_next_events() {
  int i,k;
  for (i = 0; i < N; i++)
    if (next_event(i)) {
      switch (q[i].etype) { 
      case INTERNAL:
	print_internal_event(i, q[i].vc);
	break;
      case SEND:
	print_send_message(i, q[i].ed.se.receiver, q[i].vc);
	break;	
      default:
	print_receive_message(i, q[i].ed.re.sender, q[i].ed.re.rvc, q[i].vc);
	break;
      }
      for (k = 0; k < N; k++)
	if (vc[k] < q[i].vc[k])
	  vc[k] = q[i].vc[k];
      q[i].etype = NONE;
    }
}

void init() {
  int i;
  for (i = 0; i < N; i++)
    q[i].etype = NONE;
}

int main(int argc, char* argv[])  {
  get_monitor_queues();
  init();

  while(1) {
    receive_messages();
    monitor_next_events();
  }

  return 0;
} 
