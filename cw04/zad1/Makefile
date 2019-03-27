CC = gcc
CFLAGS = -Wall

.PHONY: clean

all : sig_test sig_test2
default : all

sig_test : sig_test.c
	$(CC) $(CFLAGS) sig_test.c -o sig_test

sig_test2 : sig_test2.c
	$(CC) $(CFLAGS) sig_test2.c -o sig_test2

test : sig_test
	./sig_test

test2 : sig_test2
	./sig_test2

clean :
	rm -f sig_test
	rm -f sig_test2