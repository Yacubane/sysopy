CC = gcc
CFLAGS = -Wall
HEADERS = 
SRC = 

.PHONY: clean

all : main

client : $(SRC) $(HEADERS) client.c
	$(CC) $(CFLAGS) $(SRC) client.c -o client

server : $(SRC) $(HEADERS) server.c
	$(CC) $(CFLAGS) $(SRC) server.c -o server

clean :
	rm -f client
	rm -f server

