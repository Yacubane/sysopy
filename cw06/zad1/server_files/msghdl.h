#pragma once
#include "../shared/errors.h"
#include "msghdl_spec.h"
#include "../shared/utils.h"
#include "../shared/shared.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TYPE_STOP 0
#define TYPE_LIST 1
#define TYPE_FRIENDS 2
#define TYPE_ADD 3
#define TYPE_DEL 4
#define TYPE_ECHO 5
#define TYPE_2ALL 6
#define TYPE_2FRIENDS 7
#define TYPE_2ONE 8
#define TYPE_INIT 9

int init_msghdl();
void handle_message(int client_id, int type, char* message);

