CC = gcc
CFLAGS = -Wall

all : main
default : all

main : main.c files.c reporting.c
	$(CC) $(CFLAGS) main.c files.c reporting.c -o main

generate : main
	./generate.sh

clean :
	rm -f data*
	rm -f main
	rm -f tmp
	rm -f wyniki_gen.txt