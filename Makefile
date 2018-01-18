# -*- MakeFile -*-

CC=gcc
CFLAGS= -Wall -pedantic -g -O0

all: prog

prog: main.o cleancode.o logger.o
	$(CC) -o prog $(CFLAGS) main.o cleancode.o logger.o

main.o: main.c cleancode.h logger.h
	$(CC) -c  $(CFLAGS) main.c

cleancode2.o: cleancode.c cleancode.h
	$(CC) -c  $(CFLAGS) cleancode.c

logger.o: logger.c logger.h
	$(CC) -c $(CFLAGS) logger.c

clean:
	rm -rf *.o prog
