CC = gcc
CFLAGS = -Wall -g

PROGRAMS = remove-queues vc

all: $(PROGRAMS)

vc: ds.o vc.c

remove-queues: ds.h remove-queues.c

clean:
	rm -f *.o *~ $(PROGRAMS)
