#include <stdlib.h>
#include <errno.h>
#include "errors.h"
#include <time.h>

int parse_num(const char *string, int *out)
{
    errno = 0;
    char *end = NULL;
    int tmp = (int)strtol(string, &end, 10);
    if (errno != 0 || end == string)
        return err("Parsing num failed", -1);
    *out = tmp;
    return 0;
}

void current_time(char *buffer, int size)
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, size, "%Y-%m-%d %H-%M-%S", timeinfo);
}