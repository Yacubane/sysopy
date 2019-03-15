#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "files.h"

static int create_error(char* message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}

time_t parse_date(char *date, char *time)
{

    char full_date[255];
    full_date[0] = '\0';
    strcat(full_date, date);
    strcat(full_date, " ");
    strcat(full_date, time);

    struct tm tm ={0};

    char* end = strptime(full_date, "%Y-%m-%d %H:%M", &tm);
    if (end == NULL) 
    {
        fprintf(stderr, "Date parse error\n");
        return -1;

    } else if (*end != '\0') 
    {
        fprintf(stderr, "Date parse error (too much data?)\n");
        return -1;
    }
    return mktime(&tm); 
}

int main(int argc, char *argv[])
{
    if(argc != 5)
        return create_error("You must run this program with 4 arguments: path, sign, date and time");
    
    char* path = argv[1];
    char* sign = argv[2];
    char* date = argv[3];
    char* time = argv[4];

    time_t final_time = parse_date(date, time);

    if(final_time == -1) {
        fprintf(stderr, "Date parse error\n");
        return -1;
    }

    int final_sign = 0;
    char first_letter = sign[0];
    if(first_letter == '<')
        final_sign = -1;
    else if(first_letter == '=' ){
        final_sign = 0;
    }else if(first_letter == '>'){
        final_sign = 1;
    } else {
        fprintf(stderr, "Sign must be <, = or >\n");
        return -1;
    }

    printf("%-10s %-12s %-16s %-16s %s\n", "Type", "Size", "Accessed", "Modified", "Path");
    view_dir(path, final_sign, final_time);

    return 0;
}