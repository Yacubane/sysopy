#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "finder.h"
#include <errno.h>
#include <time.h>
#include <sys/times.h>

#define ESUCCESS 0
#define EPARSE_ERROR_TYPE_MISMATCH -1
#define EPARSE_ERROR_WRONG_NUMBER_OF_ARGUMENTS -2
#define EPARSE_ERROR_UNDEFINED_OPERATION -3

struct timespec start, stop;
struct tms start_tms, stop_tms;

struct timespec diff(struct timespec start, struct timespec stop)
{
	struct timespec temp;
	if ((stop.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = stop.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+stop.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = stop.tv_sec-start.tv_sec;
		temp.tv_nsec = stop.tv_nsec-start.tv_nsec;
	}
	return temp;
}

struct tms diff_tms(struct tms start, struct tms end) {
    struct tms temp;
    temp.tms_cstime = end.tms_cstime - start.tms_cstime;
    temp.tms_cutime = end.tms_cutime - start.tms_cutime;
    temp.tms_stime = end.tms_stime - start.tms_stime;
    temp.tms_utime = end.tms_utime - start.tms_utime;

    return temp;
}

void start_timer()
{
    times(&start_tms);
    clock_gettime(CLOCK_REALTIME, &start);
}

void stop_timer(char* text)
{
    clock_t test = times(&stop_tms);
    clock_gettime(CLOCK_REALTIME, &stop);

    struct timespec diff_time = diff(start, stop);
    struct tms diff_time_tms = diff_tms(start_tms, stop_tms);


    printf("%s %ld %ld %ld %ld %ld %ld\n", text,diff_time.tv_sec, diff_time.tv_nsec, 
            stop_tms.tms_utime, stop_tms.tms_stime,
            diff_time_tms.tms_cutime, diff_time_tms.tms_cstime);
}

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

        start_timer();
        create_table(number);
        stop_timer("create_table");

        return index + 1;
    } 
    else if(!strcmp(argv[index], "search_directory"))
    {
        if(check_argument_size(index, 3, argc)) 
            return EPARSE_ERROR_WRONG_NUMBER_OF_ARGUMENTS; 

        char* dir = argv[index+1];
        char* file = argv[index+2];
        char* name_file_temp = argv[index+3];

        start_timer();
        set_search(dir, file, name_file_temp);
        search_directory();
        stop_timer("search_directory");

        return index + 3;
    } 
    else if(!strcmp(argv[index], "save_last_result"))
    {
        start_timer();
        save_last_result();
        stop_timer("save_last_result");

        return index;
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

        start_timer();
        remove_data_block(number);
        stop_timer("remove_data_block");

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