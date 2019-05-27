#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared_files/config.h"
#include "../shared_files/errors.h"
#include "../shared_files/global_msgtypes.h"
#include "../shared_files/utils.h"
#include "msghdl_spec.h"

int init_msghdl();
void handle_message(int client_id, int type, char* message);
