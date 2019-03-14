#include "reporting.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

static struct timespec timespec_start, timespec_stop;
static struct tms tms_start, tms_stop;

void start_reporting()
{

    char buffer[255]; 
    sprintf(buffer, "%9s %9s %9s %9s %9s %9s\n",
            "Real[s]", "Real[n]", 
            "utime[s]", "stime[s]", "cutime[s]", "cstime[s]");
    print_report_text(buffer);
}

void print_report_text(char* content) {
    printf("%s", content);
}


struct timespec timespec_diff(struct timespec start, struct timespec stop)
{
	struct timespec temp;
	if ((stop.tv_nsec-start.tv_nsec) < 0) 
    {
		temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + stop.tv_nsec - start.tv_nsec;
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

void stop_report_timer()
{
    times(&tms_stop);
    clock_gettime(CLOCK_REALTIME, &timespec_stop);

    struct tms tms_diff_time = tms_diff(tms_start, tms_stop);
    struct timespec timespec_diff_time = timespec_diff(timespec_start, timespec_stop);

    long clktck = sysconf(_SC_CLK_TCK);

    char buffer[255]; 

    sprintf(buffer, "%9ld %09ld %9.2f %9.2f %9.2f %9.2f\n",
            timespec_diff_time.tv_sec, timespec_diff_time.tv_nsec, 
            tms_diff_time.tms_utime / (double) clktck,
            tms_diff_time.tms_stime / (double) clktck,
            tms_diff_time.tms_cutime / (double) clktck,
            tms_diff_time.tms_cstime / (double) clktck);

    print_report_text(buffer);
}
