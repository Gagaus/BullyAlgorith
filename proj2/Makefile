CC = gcc
CFLAGS = -Wall -g

PROGRAMS = monitor processo ds

all: $(PROGRAMS)

processo: ds.o processo.c

monitor: ds.o monitor.c

clean:
	rm -f *.o *~ $(PROGRAMS)
