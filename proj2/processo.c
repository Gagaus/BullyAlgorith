#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ds.h"

#define TIMEOUT 10
#define SLEEP_TIME 1
#define TIME_OF_DEATH 10
#define NUM_PROCESS 6

long pid;    /* Process' id */
char data[DATASIZE]; /* Process' data */
int leader = 6; // guarda o lider do ds


void get_pid_from_argv(int argc, char* argv[]) {

  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    exit(1);
  }

  sscanf(argv[1], "%ld", &pid);

  if (pid >= N) {
    printf("Pid %ld out of bounds.\n", pid);
    exit(1);
  }
}


int main(int argc, char* argv[]) {
	get_pid_from_argv(argc, argv);
	state pState = (pid==6) ? LEADER : IDLE;
	printf("Sou o processo %ld\n", pid);
	Msgbuf inbuf, outbuf;
	outbuf.mtype = pid;
	get_pid_from_argv(argc, argv);
	
	get_queues();
	
	srand(time(NULL));
	
	while(1){
		int e = rand() % 50;
		if (e==pid){
			pState = DEAD;
			outbuf.c = DEAD_WARNING;
			outbuf.mtype = pid;
			send_message(MONITOR_PID, &outbuf, sizeof(Msgbuf));
			printf("p%ld: MORRI\n", pid);
		}
		else if (pState == IDLE){
			if (e>=10 && e<=25){ // manda msg pro lider
				pState = WAITING_LEADER;
				outbuf.c = GENERIC;
				outbuf.mtype = pid;
				send_message(leader, &outbuf, sizeof(Msgbuf));
				printf ("p%ld: Mandei generic pro lider\n", pid);
			}
			else{
				printf("p%ld: Vou dormir e verificar se a mensagens\n", pid);
				sleep (SLEEP_TIME);
				if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) == 0){
					printf("p%ld: Tem mensagem pra mim!\n", pid);
					if (inbuf.c == COORDINATOR){
						leader = inbuf.mtype; // novo lider eh quem mandou a mensagem
					}
					else if (inbuf.c == ELECTION && inbuf.mtype < pid){
						outbuf.c = OK;
						outbuf.mtype = pid;
						send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
						pState = CALL_ELECTION;
					}
				}				
			}			
		}
		else if (pState == LEADER) {
			printf("p%ld: Entrei no modo lider!\n", pid);
			if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) < 0) { // se nao tem mensagem espera e entao continua o processo 
				sleep(SLEEP_TIME);								
			} else if (inbuf.c == ELECTION && inbuf.mtype < pid) { // processa a mensagem					
				outbuf.c = OK;
				outbuf.mtype = pid;
				send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));					
			} else if (inbuf.c == COORDINATOR && inbuf.mtype != pid) {
				leader = inbuf.mtype;
				pState = IDLE;
			} else if (inbuf.c == GENERIC) {
				outbuf.c = GENERIC;
				outbuf.mtype = pid;
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
					break;
				}
				else if (inbuf.c == ELECTION && inbuf.mtype < pid){
					outbuf.c = OK;
					outbuf.mtype = pid;
					send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
					pState = CALL_ELECTION;
					break;
				}
				else if (inbuf.c == GENERIC){ // lider respondeu! ele ainda esta vivo!!
					pState = IDLE;		
					break;			
				}
			}
		}
		else if (pState == CALL_ELECTION){
			//pedi eleicao
			printf("p%ld: vou pedir eleicao\n", pid);
			int j;
			for (j = pid+1; j <= NUM_PROCESS; j++){
				outbuf.c = ELECTION;
				outbuf.mtype = pid;
				send_message(j, &outbuf, sizeof(Msgbuf));
			}
			int rcv = -1;
			int k = 0;
			while (k < TIMEOUT){
					k++;
					//printf("p%ld: Entrei no while d CALL_ELECTION\n", pid);
					rcv = nowait_receive_message(pid, &inbuf, sizeof(Msgbuf));
					//printf ("p%ld: Recebi a mensagem: %d, de %ld\n", pid, inbuf.c, inbuf.mtype);
					if (rcv == 0 && inbuf.c == COORDINATOR)
						leader = inbuf.mtype; 
					else if (rcv == 0 && inbuf.c == ELECTION && inbuf.mtype < pid){
						outbuf.c = OK;
						printf("p%ld: vou mandar um OK\n", pid);
						outbuf.mtype = pid;
						send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
					}
					if (rcv == 0 && inbuf.c == OK)
						break;
					sleep(SLEEP_TIME);
					rcv = -1;
			}
			if (rcv < 0) { // se nao ha mensagens pra mim, o lider sou eu! :D
				printf("p%ld: sou o lider! vou fazer o broadcast\n", pid);
				leader = pid;
				outbuf.c = COORDINATOR;
				outbuf.mtype = pid;
				broadcast(&outbuf, sizeof(outbuf));
				pState = LEADER;
			}
			else				
				pState = IDLE; // se ha mensagem do tipo OK, minha parte acabou
		}
		if (pState == DEAD){
			printf("p%ld: Continuo dead :(\n", pid);
			sleep(TIME_OF_DEATH);
			while (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) != -1)
				{} // zera minha fila
			pState = CALL_ELECTION;
		}
	}
	return 0;
	}
	
