#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ds.h"

#define TIMEOUT 10
#define SLEEP_TIME 1
#define TIME_OF_DEATH 40
#define NUM_PROCESS 6

long pid;    /* Process' id */
char data[DATASIZE]; /* Process' data */
int leader = 6; // guarda o lider do ds


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
	outbuf.mtype = pid;
	int idLeader;
	get_pid_from_argv(argc, argv);
	
	get_queues();
	
	srand(time(NULL));
	
	while(true){
		int e = rand() % 100;
		if (e==1){
			pState = DEAD;
		}
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
			if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) < 0){ // se nao tem mensagem espera e entao continua o processo 
				sleep(SLEEP_TIME);								
			} else if (inbuf.c == ELECTION && inbuf.mtype < pid) { // processa a mensagem					
				outbuf.c = OK;
				send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
				pState = CALL_ELECTION;						
			} else if (inbuf.c == COORDINATOR) {
				leader = inbuf.mtype;
				pState = IDLE;
			} else if (inbuf.c == GENERIC) {
				outbuf.c = GENERIC;
				send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
			}	
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
				else if (inbuf.c == COORDINATOR){
					leader = inbuf.mtype;
					pState = IDLE;
				}
				else if (inbuf.c == ELECTION && inbuf.mtype < pid){
					output.c = OK;
					send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
					pState = CALL_ELECTION;
				}
				else if (inbuf.c == GENERIC){ // lider respondeu! ele ainda esta vivo!!
					pState = IDLE;					
				}
			}
		}
		else if (pState == CALL_ELECTION){
			//pedi eleicao
			int j;
			for (j = pid+1; j <= NUM_PROCESS; j++)
				send_message(j, &outbuf, sizeof(Msgbuf));
			int rcv = -1;
			time_t inicial_t = time(NULL);
			while (time(NULL) - inicial_t < TIMEOUT && rcv = nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) && rcv < 0 && inbuf.c != OK) {
					sleep(SLEEP_TIME);
					// fix the shit
			}
			if (rcv < 0) { // se nao ha mensagens pra mim, o lider sou eu! :D
				leader = pid;
				outbuf.c = COORDINATOR;
				broadcast(&outbuf, sizeof(outbuf));
			}
			else{
				// se ha mensagem do tipo OK, minha parte acabou
				pState = IDLE;
			}
		}
		else if (pState == DEAD){
			sleep(TIME_OF_DEATH);
			while (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) != -1)
				{} // zera minha fila
			pState = CALL_ELECTION;
		}
	}                 
	return 0;
	}
	
