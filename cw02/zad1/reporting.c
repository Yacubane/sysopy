#include "reporting.h"
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

static struct tms tms_start, tms_stop;

void start_reporting() {
  char buffer[255];
  sprintf(buffer, "%9s %9s\n", "utime[s]", "stime[s]");
  print_report_text(buffer);
}

void print_report_text(char* content) {
  printf("%s", content);
}

struct tms tms_diff(struct tms start, struct tms end) {
  struct tms temp;
  temp.tms_cstime = end.tms_cstime - start.tms_cstime;
  temp.tms_cutime = end.tms_cutime - start.tms_cutime;
  temp.tms_stime = end.tms_stime - start.tms_stime;
  temp.tms_utime = end.tms_utime - start.tms_utime;
  return temp;
}

void start_report_timer() {
  times(&tms_start);
}

void stop_report_timer() {
  times(&tms_stop);

  struct tms tms_diff_time = tms_diff(tms_start, tms_stop);

  long clktck = sysconf(_SC_CLK_TCK);

  char buffer[255];

  sprintf(buffer, "%9.2f %9.2f\n", tms_diff_time.tms_utime / (double)clktck,
          tms_diff_time.tms_stime / (double)clktck);

  print_report_text(buffer);
}
