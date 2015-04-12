#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ds.h"

#define TIMEOUT 10
#define SLEEP_TIME 1
#define TIME_OF_DEATH 40

long pid;    /* Process' id */
char data[DATASIZE]; /* Process' data */
int leader = 6; // guarda o lider do ds

typedef enum {ELECTION, OK, COORDINATOR, GENERIC} contentMsg;
typedef enum {IDLE, DEAD, CALL_ELECTION, WAITING_LEADER, LEADER} state;


typedef struct msgbuf {
  long mtype;      /* message type */
  contentMsg c;
} Msgbuf;

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
	state pState = IDLE;
	Msgbuf inbuf, outbuf;
	int idLeader;
	get_pid_from_argv(argc, argv);
	
	get_queues();
	
	srand(time(NULL));
	
	while(true){
		int e = rand() % 100;
		if (e==1)
			pState = DEAD;
		else if (pState == IDLE){
			if (e == 2){ // manda msg pro lider
				pState = WAITING_LEADER;
				output.c = GENERIC;
				send_message(leader, &outbuf, sizeof(Msgbuf));
			}
			else{
				sleep (SLEEP_TIME);
				if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) == 0){
					if (inbuf.c == COORDINATOR){
						leader = inbuf.mtype; // novo lider eh quem mandou a mensagem
					}
					else if (inbuf.c == ELECTION && inbuf.mtype < pid){
						output.c = OK;
						send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
						pState = CALL_ELECTION;
					}
				}
				
			}
			
		}
		else if (pState == LEADER){
			// RECEBER MENSAGENS GENERICAS. SE RECEBER, ENVIAR OUTRA DE VOLTA
		}
		else if (pState == WAITING_LEADER){
			int j = 0;
			while (j < TIMEOUT){
				if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) < 0){ // FAZER WHILE POR TEMṔO. 
					sleep(SLEEP_TIME);
					j++;
				// LIDER MORREU!! :(
				// pedir eleicao
					pState = CALL_ELECTION;
				
				}
			}
		}
		else if (pState == CALL_ELECTION){
			int j = 0;
			while (j < TIMEOUT){
				if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) < 0){ // FAZER WHILE POR TEMṔO. 
					sleep(SLEEP_TIME);
					j++;
					// se nao ha mensagens pra mim, o lider sou eu! :D
					leader = pid;
					// fazer broadcast 
				}
				else{
					// se ha mensagem do tipo OK, minha parte acabou
					pState = IDLE;
				}
			}
		}
		else if (pState == DEAD){
			sleep(TIME_OF_DEATH);
			// zerar minha fila
			// pedir eleicao
		}
	}                 
	return 0;
	}
	
