CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

master : master.c
	$(CC) $(CFLAGS) master.c -o master

slave : slave.c
	$(CC) $(CFLAGS) slave.c -o slave

test : master slave
	./test.sh

clean :
	rm -f master
	rm -f slave
	rm -f fifo
