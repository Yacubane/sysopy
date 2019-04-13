CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

server : sysv_server_files/server.c server_files/msghdl.c server_files/msghdl_spec.c sysv_server_files/clients_handler.c sysv_shared_files/msgtype_converter.c shared_files/errors.c shared_files/utils.c sysv_shared_files/config_spec.c shared_files/colors.c
	$(CC) $(CFLAGS) sysv_server_files/server.c server_files/msghdl.c server_files/msghdl_spec.c sysv_server_files/clients_handler.c  sysv_shared_files/msgtype_converter.c shared_files/errors.c shared_files/utils.c sysv_shared_files/config_spec.c shared_files/colors.c -o server -lrt

run_server : server
	./server

client : sysv_client_files/client.c sysv_shared_files/msgtype_converter.c client_files/client_helper.c client_files/cparser.c shared_files/errors.c shared_files/utils.c sysv_shared_files/config_spec.c shared_files/colors.c
	$(CC) $(CFLAGS) sysv_client_files/client.c sysv_shared_files/msgtype_converter.c client_files/client_helper.c client_files/cparser.c shared_files/errors.c shared_files/utils.c sysv_shared_files/config_spec.c shared_files/colors.c  -o client -lrt

run_client : client
	./client

clean :
	rm -f server
	rm -f client
