CC = gcc
CFLAGS = -Wall -g

PROGRAMS = remove-queues simple-broadcast

all: $(PROGRAMS)

simple-broadcast: ds.o simple-broadcast.c

remove-queues: ds.o remove-queues.c

clean:
	rm -f *.o *~ $(PROGRAMS)
