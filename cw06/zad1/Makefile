CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

server : sysv_server_files/server.c server_files/msghdl.c sysv_server_files/msghdl_spec.c shared/errors.c shared/utils.c shared/shared.c shared/colors.c
	$(CC) $(CFLAGS) sysv_server_files/server.c server_files/msghdl.c sysv_server_files/msghdl_spec.c shared/errors.c shared/utils.c shared/shared.c shared/colors.c -o server

run_server : server
	./server

client : sysv_client_files/client.c client_files/cparser.c shared/errors.c shared/utils.c shared/shared.c shared/colors.c
	$(CC) $(CFLAGS) sysv_client_files/client.c client_files/cparser.c shared/errors.c shared/utils.c shared/shared.c shared/colors.c  -o client

run_client : client
	./client A

clean :
	rm -f server
	rm -f client
