#include "shared.h"
char *get_home_dir()
{
    return getenv("HOME");
}
key_t get_server_key()
{
    return ftok(get_home_dir(), SERVER_PROJ_ID);
}
