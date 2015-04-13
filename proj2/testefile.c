#include <stdio.h>
#include <signal.h>

FILE *fp;

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    	fclose(fp);
    	exit(1);
	}
}

int main() {

  	signal(SIGINT, sig_handler);

	fp = fopen("opa", "a");
	fprintf(fp, "agora eh toes\n");

	while(1);
	fclose(fp);
	return 0;
}