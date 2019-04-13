#pragma once
#define SERVER_PROJ_ID 'S'

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

char *get_home_dir();
key_t get_server_key();