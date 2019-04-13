#pragma once
#include "../shared_files/errors.h"
#include "msghdl_spec.h"
#include "../shared_files/utils.h"
#include "../shared_files/config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../shared_files/global_msgtypes.h"

int init_msghdl();
void handle_message(int client_id, int type, char* message);

