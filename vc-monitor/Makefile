CC = gcc
CFLAGS = -Wall -g

PROGRAMS = remove-queues process monitor

all: $(PROGRAMS)

process: ds.o ds-monitor.o print-vector.o process.h process.c

monitor: ds-monitor.o print-vector.o monitor.c

remove-queues: ds.h ds-monitor.h remove-queues.c

clean:
	rm -f *.o *~ $(PROGRAMS)
