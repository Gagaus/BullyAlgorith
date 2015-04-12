/*
 * Monitor queues and message type.
 */

#include "ds-monitor.h"
#include "process.h"

#define MONITOR_KEY_PREFIX 512
#define MONITOR_MESSAGE_TYPE 1

typedef enum Event_t {INTERNAL, SEND, RECEIVE, BROADCAST} Event_t;
 
typedef struct send_t {
  int receiver;
  char data[DATASIZE];
} Send_t;

typedef struct receive_t {
  int sender;
  int rvc[N];
  char data[DATASIZE];
} Receive_t;

typedef struct monitorbuf {
  long mtype;      /* message type */
  Event_t etype;   /* event type */
  int vc[N];      /* vector clock */
  union {
    Receive_t re; /* receive event */
    Send_t se;    /* send event */
  } e;
} Monitorbuf;

void get_monitor_queue(int pid);

/* Report an internal event to the monitor */
void report_internal_event();
/* Report a send message event to the monitor */
void report_send_event(int j);
/* Report a receive message event to the monitor */
void report_receive_event(int j);
