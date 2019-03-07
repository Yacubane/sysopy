#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "finder.h"
#include <errno.h>

#define ESUCCESS 0
#define EPARSE_ERROR_TYPE_MISMATCH -1
#define EPARSE_ERROR_WRONG_NUMBER_OF_ARGUMENTS -2
#define EPARSE_ERROR_UNDEFINED_OPERATION -3

int check_argument_size(int index, int arguments_length, int argc)
{
    if(index + arguments_length >= argc) 
        return -1;
    return 0;
}
int parse_command(int index, int argc, char *argv[])
{
    if(!strcmp(argv[index], "create_table"))
    {
        if(check_argument_size(index, 1, argc)) 
            return EPARSE_ERROR_WRONG_NUMBER_OF_ARGUMENTS; 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return EPARSE_ERROR_TYPE_MISMATCH;

        create_table(number);

        return index + 1;
    } 
    else if(!strcmp(argv[index], "search_directory"))
    {
        if(check_argument_size(index, 3, argc)) 
            return EPARSE_ERROR_WRONG_NUMBER_OF_ARGUMENTS; 

        char* dir = argv[index+1];
        char* file = argv[index+2];
        char* name_file_temp = argv[index+3];

        set_search(dir, file, name_file_temp);
        search_directory();

        return index + 3;
    } 
    else if(!strcmp(argv[index], "remove_block"))
    {
        if(check_argument_size(index, 1, argc)) 
            return EPARSE_ERROR_WRONG_NUMBER_OF_ARGUMENTS; 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return EPARSE_ERROR_TYPE_MISMATCH;

        remove_data_block(number);

        return index + 1;
    } 
    else
    {
        return EPARSE_ERROR_UNDEFINED_OPERATION;
    }
    
}

int main(int argc, char *argv[])
{
    for(int i = 1; i < argc; i++) 
    {
        int parse_result = parse_command(i, argc, argv);
        if(parse_result < 0)
        {
            printf("Parse error %d", parse_result);
            return -1;
        }
        i = parse_result;
    }
    return 0;
}