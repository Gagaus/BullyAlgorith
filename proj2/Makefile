CC = gcc
CFLAGS = -Wall -g

PROGRAMS = monitor processo

all: $(PROGRAMS)

monitor: ds.o monitor.c
processo: ds.o processo.c


clean:
	rm -f *.o *~ $(PROGRAMS)
