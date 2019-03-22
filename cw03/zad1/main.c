#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"

static int create_error(char* message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}


int main(int argc, char *argv[])
{
    if(argc != 2)
        return create_error("You must run this program with 1 argument: path");
    
    char* path = argv[1];

    view_dir(path);

    return 0;
}