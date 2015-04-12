/*
 * Distributed system implementation using IPC.
 * Extended part for monitoring.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds-monitor.h"

int monitor_queue;

void get_monitor_queue(int pid) {
  if ((monitor_queue = msgget(MONITOR_KEY_PREFIX+pid, 
			      IPC_CREAT | 0700)) == -1) {
      perror("msgget error");
      exit(2);
    }
}

/* Report internal event to the monitor. */
void report_internal_event(int vc[N], char data[DATASIZE]) {
  Monitorbuf mbuf;
  int i;

  mbuf.mtype = MONITOR_MESSAGE_TYPE;
  mbuf.etype = INTERNAL;
  for (i = 0; i < N; i++)
    mbuf.vc[i] = vc[i];
  for (i = 0; i < DATASIZE; i++)
    mbuf.data[i] = data[i]; 
  if (msgsnd(monitor_queue, &mbuf, sizeof(mbuf), 0) == -1) {
    perror("monitor msgsnd error");
    exit(2);
  }
}

/* Report send message event to the monitor. */
void report_send_event(int j, Msgbuf *outbuf) {
  Monitorbuf mbuf;
  int i;

  mbuf.mtype = MONITOR_MESSAGE_TYPE;
  mbuf.etype = SEND;
  for (i = 0; i < N; i++)
    mbuf.vc[i] = outbuf->vc[i];
  mbuf.ed.se.receiver = j;
  for (i = 0; i < DATASIZE; i++)
    mbuf.data[i] = outbuf->data[i]; 
  if (msgsnd(monitor_queue, &mbuf, sizeof(mbuf), 0) == -1) {
    perror("monitor msgsnd error");
    exit(2);
  }
}

/* Report receive message event to the monitor. */
void report_receive_event(int vc[N], Msgbuf* inbuf) {
  Monitorbuf mbuf;
  int i;

  mbuf.mtype = MONITOR_MESSAGE_TYPE;
  mbuf.etype = RECEIVE;
  for (i = 0; i < N; i++) {
    mbuf.vc[i] = vc[i];
    mbuf.ed.re.rvc[i] = inbuf->vc[i];
  }
  for (i = 0; i < DATASIZE; i++)
    mbuf.data[i] = inbuf->data[i]; 
  mbuf.ed.re.sender = inbuf->sender;
  if (msgsnd(monitor_queue, &mbuf, sizeof(mbuf), 0) == -1) {
    perror("monitor msgsnd error");
    exit(2);
  }
}

