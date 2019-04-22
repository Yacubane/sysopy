CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

server : sysv_shared_files/*.c  sysv_server_files/*.c shared_files/*.c server_files/*.c
	$(CC) $(CFLAGS) sysv_shared_files/*.c  sysv_server_files/*.c shared_files/*.c server_files/*.c -o server -lrt

run_server : server
	./server

client : sysv_shared_files/*.c  sysv_client_files/*.c shared_files/*.c client_files/*.c
	$(CC) $(CFLAGS) sysv_shared_files/*.c  sysv_client_files/*.c shared_files/*.c client_files/*.c -o client -lrt


run_client : client
	./client A

clean :
	rm -f server
	rm -f client
