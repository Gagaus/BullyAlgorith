/*
 *  Distributed system implementation using IPC.
 */

#ifndef DS_H
#define DS_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define DATASIZE 10
#define N 7

#define QUEUE_KEY_PREFIX 256
#define DS_MESSAGE_TYPE 0
#define MONITOR_PID 0

/** Há cinco tipos de mensagens:
 * GENERIC testa se o lider ainda está vivo;
 * ELECTION pede eleição;
 * OK é a resposta de nós superiores a pedidos de eleição inferiores
 * COORDINATOR é a mensagens broadcasted aos processos anunciando o novo líder
 * DEAD_WARNING é enviado diretamente ao monitor quando um processo morre ou volta à vida*/
typedef enum {ELECTION, OK, COORDINATOR, GENERIC, DEAD_WARNING} contentMsg;

/** Os processos são baseados em uma máquina de estados:
 * IDLE é um processo não-líder que ainda não está esperando mensagens
 * DEAD é um processo morto
 * CALL_ELECTION é o estado no qual o processo pede eleição
 * WAITING_LEADER acontece quando um processo está aguardando uma resposta GENERIC do líder
 * LEADER é o estado que um processo assume quando é líder */
typedef enum {IDLE, DEAD, CALL_ELECTION, WAITING_LEADER, LEADER} state;

typedef struct msgbuf {
  long mtype;      /* message type */
  contentMsg c;
  int receiver;
} Msgbuf;

/* Get N communication queues */
void get_queues();  

/* Send a message to Pj. */
void send_message(int j, const void* buffer, size_t msgsize);
/* Broadcast a message to all processes, including itself. */
void broadcast(const void* buffer, size_t msgsize);
  
/* Wait for a new message */ 
void receive_message(int pid, void* buffer, size_t msgsize);
/* Returns -1 immediately if no message is available in the queue. 
   Returns 0 and a new message in buffer otherwise. */
int nowait_receive_message(int pid, void* buffer, size_t msgsize);

#endif
