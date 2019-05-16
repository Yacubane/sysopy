#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/ipc.h>

int parse_num(const char *string, int *out)
{
    errno = 0;
    char *end = NULL;
    int tmp = (int)strtol(string, &end, 10);
    if (errno != 0 || end == string)
        return -1;
    *out = tmp;
    return 0;
}