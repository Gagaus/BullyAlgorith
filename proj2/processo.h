/*
 * Distributed system implementation using IPC.
 * Extended version using vector clocks and an external monitor.
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "ds.h"
#define DATASIZE 10


/* Interprocess communication */

typedef struct msgbuf {
  long mtype;      /* message type */
  contentMsg c;
  int receiver;
} Msgbuf;

/* Process to monitor communication  */

typedef enum Event_t {NONE, INTERNAL, SEND, RECEIVE, BROADCAST} Event_t;
 
typedef struct send_t {
  int receiver;
} Send_t;

typedef struct receive_t {
  int sender;
  int rvc[N];
} Receive_t;

typedef struct monitorbuf {
  long mtype;      /* message type */
  Event_t etype;   /* event type */
  int vc[N];      /* vector clock */
  char data[DATASIZE];
  union {
    Receive_t re; /* receive event (single message or broadcast) */
    Send_t se;    /* send event */
  } ed; /* Event description */
} Monitorbuf;

#endif
