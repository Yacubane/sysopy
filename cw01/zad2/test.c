#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include "reporting.h"
#ifdef DYNAMIC
    #include "finder_dynamic.h"
#else
    #include "finder.h"
#endif

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

void start_report_timer()
{
    times(&start_tms);
    clock_gettime(CLOCK_REALTIME, &start);
}

void stop_report_timer(char* text)
{
    times(&stop_tms);
    clock_gettime(CLOCK_REALTIME, &stop);

    struct timespec diff_time = diff(start, stop);
    struct tms diff_time_tms = diff_tms(start_tms, stop_tms);

    long clktck = sysconf(_SC_CLK_TCK);

    char buffer[255]; 

    sprintf(buffer, "%-20s %9ld %12ld %6ld %6ld %6ld %6ld\n",
            text,diff_time.tv_sec, diff_time.tv_nsec, 
            diff_time_tms.tms_utime, diff_time_tms.tms_stime,
            diff_time_tms.tms_cutime, diff_time_tms.tms_cstime
            //diff_time_tms.tms_utime / (double) clktck, diff_time_tms.tms_stime / (double) clktck,
           // diff_time_tms.tms_cutime / (double) clktck, diff_time_tms.tms_cstime / (double) clktck
            );

    add_report_text(buffer);
}

void add_first_reporting_line()
{
    char buffer[255]; 

    sprintf(buffer, "%-20s %9s %12s %6s %6s %6s %6s\n",
            "Measurement", "Real[s]", "Real[n]", 
            "utime", "stime", "cutime", "cstime");

    add_report_text(buffer);
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
            return -1; 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return -1;

        create_table(number);

        return index + 1;
    } 
    else if(!strcmp(argv[index], "search_directory"))
    {
        if(check_argument_size(index, 3, argc) < 0) 
            return -1; 

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
            return -1; 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return -1;

        remove_data_block(number);

        return index + 1;
    }
    else if(!strcmp(argv[index], "get_data_block"))
    {
        if(check_argument_size(index, 1, argc) < 0) 
            return -1; 

        errno = 0;
        char* end = NULL;
        int number = (int)strtol(argv[index+1], &end, 10);

        if(errno != 0 || end == argv[index+1])
            return -1;

        get_data_block(number);

        return index + 1;
    }
    else if(!strcmp(argv[index], "start_reporting"))
    {
        if(check_argument_size(index, 1, argc) < 0) 
            return -1; 

        char* filename = argv[index+1];

        start_reporting(filename);
        add_first_reporting_line();

        return index + 1;
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
            return -1; 

        char* text = argv[index+1];
        stop_report_timer(text);
        return index+1;
    }  
    else
    {
        return -1;
    }
    
}

int main(int argc, char *argv[])
{
    #ifdef DYNAMIC
        if(init_dynamic() < 0)
            exit(1);
    #endif

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