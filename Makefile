#
# Makefile for project 2 (COMP30023)
#
# Kemble Song (584999) (kembles)
#
#


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall

OBJ = server.o serverfunc.o client.o clientfunc.o
EXE = server client

## Top level target is executable.
all: server client

server:	server.o serverfunc.o
		$(CC) $(CFLAGS) -o server server.o serverfunc.o -lpthread

client:	client.o clientfunc.o
		$(CC) $(CFLAGS) -o client client.o clientfunc.o


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ)

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies
server.o:	server.c server.h
serverfunc.o: serverfunc.c server.h
client.o: client.c	client.h
clientfunc.o: clientfunc.c client.h
