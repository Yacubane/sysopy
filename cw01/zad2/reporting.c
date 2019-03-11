#include "reporting.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>


static int reporting_status = 0;
static int fd;

static struct timespec start, stop;
static struct tms start_tms, stop_tms;

static int create_error(char* message) {
    fprintf(stderr, "%s\n", message);
    return -1;
}

int start_reporting(char* filename, char* title, int flag)
{
    if(reporting_status==1)
        return create_error("Reporting is already started");

    int realFlag = 0;
    if(flag == 1)
        realFlag = O_APPEND;
    else
        realFlag = O_TRUNC;

    if((fd = open(filename, O_WRONLY | O_CREAT | realFlag, 0644)) >= 0) 
    {
        char buffer[255]; 

        reporting_status = 1;

        sprintf(buffer, "%s\n", title);
        if(add_report_text(buffer) < 0) 
            return create_error("Cannot add first line to report");

        sprintf(buffer, "%-20s %9s %12s %9s %9s %9s %9s\n",
                "Measurement", "Real[s]", "Real[n]", 
                "utime[s]", "stime[s]", "cutime[s]", "cstime[s]");
        if(add_report_text(buffer) < 0)
            return create_error("Cannot add second line to report");

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
        return create_error("Reporting is already stopped");

    add_report_text("\n");

    reporting_status = 0;
    close(fd);

    return 0;    
}


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
    struct tms diff_time_tms = diff_tms(start_tms, stop_tms);

    clock_gettime(CLOCK_REALTIME, &stop);
    struct timespec diff_time = diff(start, stop);

    long clktck = sysconf(_SC_CLK_TCK);

    char buffer[255]; 

    sprintf(buffer, "%-20s %9ld %12ld %9.2f %9.2f %9.2f %9.2f\n",
            text,diff_time.tv_sec, diff_time.tv_nsec, 
            diff_time_tms.tms_utime / (double) clktck,
            diff_time_tms.tms_stime / (double) clktck,
            diff_time_tms.tms_cutime / (double) clktck,
            diff_time_tms.tms_cstime / (double) clktck);

    add_report_text(buffer);
}