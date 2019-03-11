#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "reporting.h"
#ifdef DYNAMIC
    #include "finder_dynamic.h"
#else
    #include "finder.h"
#endif

static int create_error(char* message) {
    fprintf(stderr, "%s\n", message);
    return -1;
}

int check_argument_size(int index, int arguments_length, int argc)
{
    if(index + arguments_length >= argc) {
        fprintf(stderr, "Wrong number of arguments");
        return -1;
    }
    return 0;
}

int parse_command(int index, int argc, char *argv[])
{
    if(!strcmp(argv[index], "create_table"))
    {
        if(check_argument_size(index, 1, argc) < 0) 
            return create_error("Error parsing create_table, wrong arguments size"); 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return create_error("Error parsing create_table, argument must be a number"); 

        create_table(number);

        return index + 1;
    } 
    else if(!strcmp(argv[index], "search_directory"))
    {
        if(check_argument_size(index, 3, argc) < 0) 
            return create_error("Error parsing search_directory, wrong arguments size"); 

        char* dir = argv[index+1];
        char* file = argv[index+2];
        char* name_file_temp = argv[index+3];

        set_search(dir, file, name_file_temp);
        search_directory();

        return index + 3;
    } 
    else if(!strcmp(argv[index], "store_last_result"))
    {
        store_last_result();

        return index;
    } 
    else if(!strcmp(argv[index], "remove_data_block"))
    {
        if(check_argument_size(index, 1, argc) < 0) 
            return create_error("Error parsing remove_data_block, wrong arguments size"); 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return create_error("Error parsing remove_data_block, argument must be a number"); 

        remove_data_block(number);

        return index + 1;
    }
    else if(!strcmp(argv[index], "get_data_block"))
    {
        if(check_argument_size(index, 1, argc) < 0) 
            return create_error("Error parsing get_data_block, wrong arguments size"); 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return create_error("Error parsing get_data_block, argument must be a number"); 

        get_data_block(number);

        return index + 1;
    }
    else if(!strcmp(argv[index], "start_reporting"))
    {
        if(check_argument_size(index, 3, argc) < 0) 
            return create_error("Error parsing start_reporting, wrong arguments size"); 

        char* filename = argv[index+1];
        char* title = argv[index+2];

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+3], &end, 10);

        if(errno != 0 || end == argv[index+3])
            return create_error("Error parsing start_reporting, third argument must be a number"); 

        start_reporting(filename, title, number);

        return index + 3;
    }  
    else if(!strcmp(argv[index], "stop_reporting"))
    {
        stop_reporting();
        return index;
    }  
    else if(!strcmp(argv[index], "start_report_timer"))
    {
        start_report_timer();
        return index;
    }  
    else if(!strcmp(argv[index], "stop_report_timer"))
    {
        if(check_argument_size(index, 1, argc) < 0) 
            return create_error("Error parsing stop_report_timer, wrong arguments size"); 

        char* text = argv[index+1];
        stop_report_timer(text);
        return index+1;
    }  
    else
    {
        fprintf(stderr, "Error parsing %s - undefined command", argv[index]);
        return -1;
    }
    
}

int main(int argc, char *argv[])
{
    #ifdef DYNAMIC
        if(init_dynamic() < 0){
            fprintf(stderr, "Dynamic library loading error, stopping\n");
            exit(1);
        }
    #endif

    for(int i = 1; i < argc; i++) 
    {
        int parse_result = parse_command(i, argc, argv);
        if(parse_result < 0)
        {
            fprintf(stderr, "Parse error, stopping\n");
            return -1;
        }
        i = parse_result;
    }
    return 0;
}