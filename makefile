CC=gcc
CFLAGS=-c -std=c99 -Wall
LFLAGS=

all: shell

shell : shell.o filesys.o
	$(CC) $(LFLAGS) -o shell shell.o filesys.o

shell.o : shell.c filesys.h
	$(CC) $(CFLAGS) shell.c

filesys.o : filesys.c filesys.h
	$(CC) $(CFLAGS) filesys.c

clean :
	rm -f *.o

