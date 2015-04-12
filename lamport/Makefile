CC = gcc
CFLAGS = -Wall -g

PROGRAMS = remove-queues lamport

all: $(PROGRAMS)

lamport: ds.o lamport.c

remove-queues: ds.h remove-queues.c

clean:
	rm -f *.o *~ $(PROGRAMS)
