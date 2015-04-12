#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "process.h"
#include "print-vector.h"
#include "ds.h"
#include "ds-monitor.h"

int pid;    /* Process' id */
int vc[N];  /* Process' vector clock */
char data[DATASIZE]; /* Process' data */

Msgbuf inbuf, outbuf;

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

  get_pid_from_argv(argc, argv);

  get_queues();
  get_monitor_queue(pid);

  srand(time(NULL));

  for (l = 0; l < 10; l++)  {
    int e = rand() % 3;
    switch (e) {
    case 0:
      vc[pid]++;  
      print_internal_event(pid, vc);
      report_internal_event(vc, data);
      break;
    case 1:
      vc[pid]++;
      outbuf.mtype = 1;
      for (i = 0; i < N; i++)
	outbuf.vc[i] = vc[i];
      outbuf.sender = pid;
      while ((j = rand() % N) == pid); 
      send_message(j, &outbuf, sizeof(Msgbuf));
      print_send_message(pid, j, vc);
      report_send_event(j, &outbuf);
      break;
    default:
      if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) == 0) {
	for (i = 0; i < N; i++)
	  if (inbuf.vc[i] > vc[i])
	    vc[i] = inbuf.vc[i];
        vc[pid]++;
	print_receive_message(pid, inbuf.sender, inbuf.vc, vc);
	report_receive_event(vc, &inbuf);  
      }
    }
  }                 
  return 0;
}
