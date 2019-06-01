CC = gcc
CFLAGS = -Wall
HEADERS = 
SRC = 

.PHONY: clean

all : main

client : $(SRC) $(HEADERS) client.c queue.c errors.c utils.c
	$(CC) $(CFLAGS) $(SRC) client.c queue.c errors.c utils.c -o client -pthread -lm

server : $(SRC) $(HEADERS) server.c errors.c utils.c
	$(CC) $(CFLAGS) $(SRC) server.c errors.c utils.c -o server -pthread -lm

clean :
	rm -f client
	rm -f server

