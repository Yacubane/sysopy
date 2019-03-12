#include "reporting.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>

static int reporting_status = 0;
static int fd;

static struct timespec timespec_start, timespec_stop;
static struct tms tms_start, tms_stop;

static int create_error(char* message) {
    fprintf(stderr, "%s\n", message);
    return -1;
}

int start_reporting(char* filename, char* title, int append)
{
    if(reporting_status==1)
        return create_error("Reporting already started");

    int flag = 0;
    if(append == 1)
        flag = O_APPEND;
    else
        flag = O_TRUNC;

    if((fd = open(filename, O_WRONLY | O_CREAT | flag, 0644)) >= 0) 
    {
        reporting_status = 1;

        char buffer[255]; 

        sprintf(buffer, "%s\n", title);
        if(add_report_text(buffer) < 0)  {
            reporting_status = 0;
            close(fd);
            return create_error("Cannot add first line to report");
        }

        sprintf(buffer, "%-20s %9s %9s %9s %9s %9s %9s\n",
                "Measurement", "Real[s]", "Real[n]", 
                "utime[s]", "stime[s]", "cutime[s]", "cstime[s]");
        if(add_report_text(buffer) < 0)  {
            reporting_status = 0;
            close(fd);
            return create_error("Cannot add second line to report");
        }


        return 0;
    } else
        return create_error("Cannot create report file");   
}

int add_report_text(char* content) {
    if(reporting_status==0)
        return -2;
    
    printf("%s", content);

    int string_size = strlen(content);
    if(write(fd, content, string_size) != string_size) {
        reporting_status = 0;
        close(fd);
        return create_error("Error appeared during writing to report file");
    }
    return 0;

}

int stop_reporting() {
    if(reporting_status==0)
        return create_error("Reporting already stopped");

    add_report_text("\n");

    reporting_status = 0;
    close(fd);

    return 0;    
}


struct timespec timespec_diff(struct timespec start, struct timespec stop)
{
	struct timespec temp;
	if ((stop.tv_nsec-start.tv_nsec)<0) 
    {
		temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + stop.tv_nsec-start.tv_nsec;
	} else 
    {
		temp.tv_sec = stop.tv_sec - start.tv_sec;
		temp.tv_nsec = stop.tv_nsec -  start.tv_nsec;
	}
	return temp;
}

struct tms tms_diff(struct tms start, struct tms end) 
{
    struct tms temp;
    temp.tms_cstime = end.tms_cstime - start.tms_cstime;
    temp.tms_cutime = end.tms_cutime - start.tms_cutime;
    temp.tms_stime = end.tms_stime - start.tms_stime;
    temp.tms_utime = end.tms_utime - start.tms_utime;
    return temp;
}

void start_report_timer()
{
    times(&tms_start);
    clock_gettime(CLOCK_REALTIME, &timespec_start);
}

int stop_report_timer(char* text)
{
    times(&tms_stop);
    clock_gettime(CLOCK_REALTIME, &timespec_stop);

    struct tms tms_diff_time = tms_diff(tms_start, tms_stop);
    struct timespec timespec_diff_time = timespec_diff(timespec_start, timespec_stop);

    long clktck = sysconf(_SC_CLK_TCK);

    char buffer[255]; 

    sprintf(buffer, "%-20s %9ld %09ld %9.2f %9.2f %9.2f %9.2f\n",
            text, timespec_diff_time.tv_sec, timespec_diff_time.tv_nsec, 
            tms_diff_time.tms_utime / (double) clktck,
            tms_diff_time.tms_stime / (double) clktck,
            tms_diff_time.tms_cutime / (double) clktck,
            tms_diff_time.tms_cstime / (double) clktck);

    return add_report_text(buffer);
}
