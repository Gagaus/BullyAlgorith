/*
 * Vector clock implementation.
 */

void print_vclock(int vc[N]);

void print_internal_event(int pid, int vc[N]);

void print_send_message(int pid, int j, int vc[N]);

void print_receive_message(int pid, int j, int rvc[N], int vc[N]);
