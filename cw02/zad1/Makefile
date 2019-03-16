CC = gcc
CFLAGS = -Wall

all : main
default : all
.PHONY : test

main : main.c files.c reporting.c
	$(CC) $(CFLAGS) main.c files.c reporting.c -o main

test : main
	./test.sh

clean :
	rm -f data*
	rm -f main
	rm -f tmp
	rm -f wyniki_gen.txt