/*
 * Remove N communication queues and N monitor queues. 
 * Must be executed by a single process.
 */

#include "ds-monitor.h"
#include "ds.h"
#include <stdlib.h>

void remove_queues();

int main() {
  int i, id;

  for (i = 0; i < N; i++) {
    if ((id = msgget(QUEUE_KEY_PREFIX+i, 0700)) != -1)
      msgctl(id, IPC_RMID, NULL);
    if ((id = msgget(MONITOR_KEY_PREFIX+i, 0700)) != -1)
      msgctl(id, IPC_RMID, NULL);
  }
  return 0;
}
