/*
 * Sends a message to a child process using IPC.
 */
#include <sys/types.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>
#include <stdio.h>

#define MY_MSGTYPE 1

typedef struct msgbuf {
  long mtype;      /* type of message  */
  char mtext[1];   /* message text .. why is this not a ptr? */
} Msgbuf;

#define MSG_SIZE sizeof(Msgbuf)

Msgbuf inbuf, outbuf;

int main() {
  int queue_id;

  if ((queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0700)) == -1)
    perror("msgget error");

  if (fork()) { 
    outbuf.mtype = MY_MSGTYPE;
    outbuf.mtext[0] = 'G';
  
    if (msgsnd(queue_id, &outbuf, MSG_SIZE, 0) == -1)
      perror("msgsnd error");

    printf("Sent character: %c\n", outbuf.mtext[0]);
  }
  else { /* Child process */
    if (msgrcv(queue_id, &inbuf, MSG_SIZE, 
	       MY_MSGTYPE, MSG_NOERROR) == -1)
      perror("msgrcv error");
    
    printf("Received character: %c\n", inbuf.mtext[0]);
  }
  
  return 0;
}
