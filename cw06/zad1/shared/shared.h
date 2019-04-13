#pragma once
#define SERVER_PROJ_ID 'S'
#define MAX_STATEMENT_SIZE 256
#define MAX_CLIENTS_SIZE 64

#include "utils.h"
#include "shared.h"
#include "errors.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

char *get_home_dir();
key_t get_server_key();