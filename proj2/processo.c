#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ds.h"

#define TIMEOUT 10 // tempo de espera antes de tomar decisoes
#define SLEEP_TIME 1
#define TIME_OF_DEATH 20 // tempo que um processo fica morto
#define NUM_PROCESS 6

long pid;    /* Process' id */
int leader = NUM_PROCESS; // guarda o lider do ds. No inicio é o processo 6


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
	state pState = (pid==6) ? LEADER : IDLE; // seta o estado inicial
	printf("Sou o processo %ld\n", pid);
	Msgbuf inbuf, outbuf;
	outbuf.mtype = pid;
	get_pid_from_argv(argc, argv);
	
	get_queues();
	
	srand(time(NULL));
	
	while(1){
		int e = rand() % 50;
		if (e==pid || (pid == leader && e < 5)){ // Processos comuns tem chance de 2% de morrer. O lider é biased
			pState = DEAD;
			outbuf.c = DEAD_WARNING;
			outbuf.mtype = pid;
			send_message(MONITOR_PID, &outbuf, sizeof(Msgbuf)); // avisa o monitor de que morreu
		}
		
		/* Agora acoes exclusivas por processo : */
		
		else if (pState == IDLE){
			if (e>=10 && e<=25){ // chance de 30% de mandar msg pro lider
				pState = WAITING_LEADER;
				outbuf.c = GENERIC;
				outbuf.mtype = pid;
				send_message(leader, &outbuf, sizeof(Msgbuf));
			}
			else{ // senao, soh verifica se ha mensagens pra ele
				sleep (SLEEP_TIME);
				if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) == 0){
					if (inbuf.c == COORDINATOR){
						leader = inbuf.mtype; // novo lider eh quem mandou a mensagem
					}
					else if (inbuf.c == ELECTION && inbuf.mtype < pid){ // se for ELECTION de um processo menor, preciso responder OK
						outbuf.c = OK;
						outbuf.mtype = pid;
						send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
						pState = CALL_ELECTION;
					}
				}				
			}			
		}
		
		else if (pState == LEADER) {
			if (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) < 0) { // se nao tem mensagem espera e entao continua o processo 
				sleep(SLEEP_TIME);								
			} else if (inbuf.c == ELECTION && inbuf.mtype < pid) { // processa a mensagem					
				outbuf.c = OK;
				outbuf.mtype = pid;
				send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));					
			} else if (inbuf.c == COORDINATOR && inbuf.mtype != pid) {
				leader = inbuf.mtype;
				pState = IDLE;
			} else if (inbuf.c == GENERIC) { // se for GENERIC, precisa responder para mostrar que estah vivo
				outbuf.c = GENERIC;
				outbuf.mtype = pid;
				send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
			}	
		}
		else if (pState == WAITING_LEADER){ // aguardando resposta generica do lider
			int j = 0;
			while (j < TIMEOUT/2){
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
			//pede eleicao
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
					rcv = nowait_receive_message(pid, &inbuf, sizeof(Msgbuf));
					if (rcv == 0 && inbuf.c == COORDINATOR)
						leader = inbuf.mtype; // recebeu coordinator, entao ha novo lider. O processo continua, porem
					else if (rcv == 0 && inbuf.c == ELECTION && inbuf.mtype < pid){ // ainda preciso mandar OKs
						outbuf.c = OK;
						outbuf.mtype = pid;
						send_message(inbuf.mtype, &outbuf, sizeof(Msgbuf));
					}
					if (rcv == 0 && inbuf.c == OK)
						break;
					sleep(SLEEP_TIME);
					rcv = -1;
			}
			if (rcv < 0) { // se nao ha mensagens pra mim, o lider sou eu! :D
				leader = pid;
				outbuf.c = COORDINATOR;
				outbuf.mtype = pid;
				broadcast(&outbuf, sizeof(outbuf)); // faz o broadcast
				pState = LEADER;
			}
			else				
				pState = IDLE; // se ha mensagem do tipo OK, minha parte acabou
		}
		if (pState == DEAD){
			sleep(TIME_OF_DEATH); // durmo o tempo que ficarei morto
			while (nowait_receive_message(pid, &inbuf, sizeof(Msgbuf)) != -1)
				{} // zera minha fila
			pState = CALL_ELECTION; // preciso pedir eleição ao acordar
			outbuf.c = DEAD_WARNING;
			outbuf.mtype = pid;
			send_message(MONITOR_PID, &outbuf, sizeof(Msgbuf)); // preciso avisar o monitor que acordei!
		}
	}
	return 0;
	}
	
