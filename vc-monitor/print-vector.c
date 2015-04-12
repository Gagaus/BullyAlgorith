/*
 * Vector clock implementation using IPC. 
 * Print vector functions.
 */

#include <stdio.h>
#include <stdlib.h>

#include "process.h"

void print_vclock(int vc[N]) {
  int i;
  printf("[");
  for (i = 0; i < N-1; i++) {
    printf("%d, ", vc[i]);
  }
  printf("%d]", vc[N-1]);
}
 
void print_internal_event(int pid, int vc[N]) {
  printf("P%d: ", pid);
  print_vclock(vc);
  printf("\n");
}

void print_send_message(int pid, int j, int vc[N]) {
  printf("P%d: --", pid);
   print_vclock(vc);
  printf("--> P%d\n", j);
}

void print_receive_message(int pid, int j, int rvc[N], int vc[N]) {
  printf("P%d: <-", pid); 
  print_vclock(rvc);
  printf("-- P%d\n", j);
  printf ("    ");
  print_vclock(vc);
  printf("\n");
}


