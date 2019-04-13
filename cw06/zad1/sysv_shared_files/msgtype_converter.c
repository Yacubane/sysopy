#include "../shared_files/global_msgtypes.h"
#include "../sysv_shared_files/local_msgtypes.h"
#include "../server_files/msghdl.h"
#include "../shared_files/msgtype_converter.h"
#include "../shared_files/config.h"

int convert_to_global(int type)
{
    switch (type)
    {
    case MTYPE_STOP:
        return TYPE_STOP;
    case MTYPE_LIST:
        return TYPE_LIST;
    case MTYPE_FRIENDS:
        return TYPE_FRIENDS;
    case MTYPE_ADD:
        return TYPE_ADD;
    case MTYPE_DEL:
        return TYPE_DEL;
    case MTYPE_ECHO:
        return TYPE_ECHO;
    case MTYPE_2ALL:
        return TYPE_2ALL;
    case MTYPE_2FRIENDS:
        return TYPE_2FRIENDS;
    case MTYPE_2ONE:
        return TYPE_2ONE;
    case MTYPE_INIT:
        return TYPE_INIT;
    }
    return -1;
}

int convert_to_local(int type)
{
    switch (type)
    {
    case TYPE_STOP:
        return MTYPE_STOP;
    case TYPE_LIST:
        return MTYPE_LIST;
    case TYPE_FRIENDS:
        return MTYPE_FRIENDS;
    case TYPE_ADD:
        return MTYPE_ADD;
    case TYPE_DEL:
        return MTYPE_DEL;
    case TYPE_ECHO:
        return MTYPE_ECHO;
    case TYPE_2ALL:
        return MTYPE_2ALL;
    case TYPE_2FRIENDS:
        return MTYPE_2FRIENDS;
    case TYPE_2ONE:
        return MTYPE_2ONE;
    case TYPE_INIT:
        return MTYPE_INIT;
    }
    return -1;
}