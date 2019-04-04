CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

main : main.c
	$(CC) $(CFLAGS) main.c -o main

test : main
	./main list.txt
clean :
	rm -f main
