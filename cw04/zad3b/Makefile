CC = gcc
CFLAGS = -Wall

.PHONY: tester test

all : main_dir main_nftw
default : all

catcher : catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher

sender : sender.c
	$(CC) $(CFLAGS) sender.c -o sender

tester : catcher sender

test : tester
	./script.sh 100 KILL
	./script.sh 100 SIGQUEUE
	./script.sh 100 SIGRT

clean :
	rm -f sender
	rm -f catcher
