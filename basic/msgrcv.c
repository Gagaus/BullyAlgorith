/*
 * Receive a message from another process using IPC.
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

Msgbuf inbuf;

int main() {
  int r;
  key_t key;
  int queue_id;

  if ((key = ftok("myqueue", 1)) == -1)
    perror("ftok error");

  if ((queue_id = msgget(key, 0700)) == -1)
    perror("msgget error");

  r = msgrcv(queue_id, &inbuf, sizeof(Msgbuf), 
	     MY_MSGTYPE, IPC_NOWAIT);
	     
  if (r == -1)
    perror("msgrcv error");
  else 
    printf("Received value: %d\n", inbuf.c);

  return 0;
}
