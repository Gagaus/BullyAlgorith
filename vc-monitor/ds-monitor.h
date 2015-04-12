/*
 * Distributed system implementation using IPC.
 * Report all events to the monitor. 
 */

#ifndef DS_MONITOR_H
#define DS_MONITOR

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MONITOR_KEY_PREFIX 512
#define MONITOR_MESSAGE_TYPE 1

#include "process.h"

void get_monitor_queue(int pid);

/* Report an internal event to the monitor */
void report_internal_event(int vc[N], char data[DATASIZE]);
/* Report a send message event to the monitor */
void report_send_event(int j, Msgbuf *outbuf);
/* Report a receive message event to the monitor */
void report_receive_event(int vc[N], Msgbuf *inbuf);

#endif
