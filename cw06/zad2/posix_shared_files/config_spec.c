#include "config_spec.h"
#include <stdio.h>
#include <unistd.h>
#include "local_msgtypes.h"
#include "shared_files/global_msgtypes.h"
char* get_server_path()
{
    return "/srv";
}

void get_client_path(char* buffer)
{
    snprintf(buffer, 50, "/%d", getpid());
}

unsigned int get_priority_by_type(int global_type)
{
    switch (global_type)
    {
    case TYPE_STOP:
        return 10;
    case TYPE_LIST:
        return 9;
    case TYPE_FRIENDS:
        return 8;
    case TYPE_ADD:
        return 7;
    case TYPE_DEL:
        return 6;
    case TYPE_ECHO:
        return 5;
    case TYPE_2ALL:
        return 4;
    case TYPE_2FRIENDS:
        return 3;
    case TYPE_2ONE:
        return 2;
    case TYPE_INIT:
        return 1;
    }
    return -1;
}