#pragma once
#define SERVER_PROJ_ID 'S'

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

char* get_home_dir();
key_t get_server_key();