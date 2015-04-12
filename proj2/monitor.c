#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// #include "ds.h"

int leader;

#define HEIGHT 15
#define WIDTH 33
int from, to;
char msg[100] = {"OK"};
int is_dead[10];
int asc_msg_size;

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
		sprintf(aux," %s→",msg);
		strcat(asc_msg, aux);
		if(!to) {
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

int main() {
	srand(time(NULL));
	memset(is_dead, 0, sizeof(is_dead));
	while(1) {
		system("clear");
		from = rand()%6 + 1;
		to = rand()%6 + 1;
		leader = rand()%6 + 1;
		print_system_status(-1,-1,'c');
		int i;
		for(i=0; i<1000000000; ++i);
	}
	return 0;
}