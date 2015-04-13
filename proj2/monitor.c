#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "ds.h"

int leader;

#define HEIGHT 15
#define WIDTH 33
#define MONITOR_TIME_STEP 1

int from, to;
const char* current_msg_content;
const char msgs[4][100] = {{"ELECTION"},{"OK"}, {"COORDINATOR"},{"GENERIC"}};
int is_dead[10];
int asc_msg_size;
FILE *fp;

int queue_id[N];

void xmove(int dx) {
	int i;
	for(i=0; i<dx; ++i) printf(" ");
}

void ymove(int dy) {
	int i;
	for(i=0; i<dy; ++i) printf("\n");
}

void printpid(int pid) {
	char pidrep;
	if(is_dead[pid]) pidrep = ' ';
	else pidrep = pid + '0';
	if(pid==leader) printf("[%c]",pidrep);
	else printf("%c",pidrep);
	if(from==pid) {
		char asc_msg[100], aux[50];
		asc_msg[0] = '\0';
		sprintf(aux," %s→ ",current_msg_content);
		strcat(asc_msg, aux);
		if(to==-1) {
			sprintf(aux,"*");
			strcat(asc_msg, aux);
		}
		else {
			sprintf(aux,"%d",to);
			strcat(asc_msg, aux);
		}	
		asc_msg_size = strlen(asc_msg)-1;
		printf("%s",asc_msg);
	}
}

void remove_queues() {
	int i, id;
	for (i = 0; i < 20; i++) {
		if ((id = msgget(QUEUE_KEY_PREFIX+i, 0700)) != -1)
			msgctl(id, IPC_RMID, NULL);
	}
}

void print_system_status() {
	int i;
	xmove(WIDTH/2 - (leader==1));
	printpid(1);
	for(i=2; i<=4; i+=2) {
		ymove(HEIGHT/3);
		printpid(i);
		xmove(WIDTH - (leader==i+1) - 2*(leader==i) - asc_msg_size*(from==i));
		printpid(i+1);
	}
	ymove(HEIGHT/3);
	xmove(WIDTH/2 - (leader==i));
	printpid(6);
	ymove(1);
}

void sig_handler(int signo) {
	if (signo == SIGINT) {
		fclose(fp);
		exit(1);
	}
}

int main() {
	remove_queues();
	// system("ipcs -a | grep 0x | awk '{printf( \"-Q %s \", $1 )}' | xargs ipcrm 2&>1 /dev/null");
	signal(SIGINT, sig_handler);
	Msgbuf inbuf;
	fp = fopen("log.txt", "w");
	leader = 6;
	memset(is_dead, 0, sizeof(is_dead));
	get_queues();
	while(1) {
		receive_message(MONITOR_PID, &inbuf, sizeof(Msgbuf));
		if(inbuf.c == DEAD_WARNING) {
			if(is_dead[inbuf.mtype]) {
				fprintf(fp,"%ld IS ALIVE\n",inbuf.mtype);
			}
			else {
				fprintf(fp,"%ld IS DEAD\n",inbuf.mtype);
			}
			is_dead[inbuf.mtype] = !is_dead[inbuf.mtype];
		}
		else {
			from = inbuf.mtype;
			to = inbuf.receiver;
			char auxto;
			if(inbuf.c == COORDINATOR) auxto = '*';
			else auxto = to + '0';
			current_msg_content = msgs[inbuf.c];
			fprintf(fp,"%d -(%s)-> %c\n",from,current_msg_content, auxto);
			is_dead[inbuf.mtype] = 0;
			if(inbuf.c == COORDINATOR) {
				to = -1;
				leader = inbuf.mtype;
			}
		}
		ymove(20);
		print_system_status();
		sleep(MONITOR_TIME_STEP);
	}
	fclose(fp);
	return 0;
}
