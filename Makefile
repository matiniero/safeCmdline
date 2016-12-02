CC = gcc
CFLAGS  = -Wall
LDFLAGS=-lreadline

default: comm

comm:  comm.c
	$(CC) $(CFLAGS) comm.c -o comm $(LDFLAGS)

all: comm

clean: 
	$(RM) comm *.o *~ 
