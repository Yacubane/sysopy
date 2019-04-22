CC = gcc
CFLAGS = -Wall -I../zad1/ -I.

.PHONY: test clean

all : test
default : all

server : posix_shared_files/*.c  posix_server_files/*.c ../zad1/shared_files/*.c ../zad1/server_files/*.c
	$(CC) $(CFLAGS) posix_shared_files/*.c  posix_server_files/*.c ../zad1/shared_files/*.c ../zad1/server_files/*.c -o server -lrt

run_server : server
	./server

client : posix_shared_files/*.c  posix_client_files/*.c ../zad1/shared_files/*.c ../zad1/client_files/*.c
	$(CC) $(CFLAGS) posix_shared_files/*.c  posix_client_files/*.c ../zad1/shared_files/*.c ../zad1/client_files/*.c -o client -lrt


run_client : client
	./client

clean :
	rm -f server
	rm -f client
