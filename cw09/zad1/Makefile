CC = gcc
CFLAGS = -Wall
HEADERS = errors.h queue.h utils.h colors.h
SRC = errors.c queue.c utils.c colors.c

.PHONY: clean

all : main

main : $(SRC) $(HEADERS) main.c
	$(CC) $(CFLAGS) $(SRC) main.c -o main -pthread

test : main
	./main 100 5 10 3

clean :
	rm -f main

