/*
 * Send a message to another process using IPC. 
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>
#include <stdio.h>

#define MY_MSGTYPE 1

typedef struct msgbuf {
  long mtype;      /* type of message  */
  int c;           /* message data     */
} Msgbuf;

Msgbuf outbuf;

int main() {
  int r;
  key_t key;
  int queue_id;

  if ((key = ftok("myqueue", 1)) == -1)
    perror("ftok error");

  if ((queue_id = msgget(key, IPC_CREAT | 0700)) == -1)
    perror("msgget error");

  outbuf.mtype = MY_MSGTYPE;
  outbuf.c = 256;
  
  r = msgsnd(queue_id, &outbuf, sizeof(Msgbuf), 0);

  if (r == -1)
    perror("msgsnd error");
  else 
    printf("Sent value: %d\n", outbuf.c);
	     
  return 0;
}
