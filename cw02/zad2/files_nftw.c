#define _XOPEN_SOURCE 500
#include <dirent.h>
#include "files.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <ftw.h>

static time_t time_;
static int sign_;
static int count;

static int create_error(char* message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}

static char* get_tile_type(const struct stat *stat_buffer) 
{
    if(S_ISREG(stat_buffer->st_mode)) return "file";
    if(S_ISDIR(stat_buffer->st_mode)) return "dir";
    if(S_ISCHR(stat_buffer->st_mode)) return "char dev";
    if(S_ISBLK(stat_buffer->st_mode)) return "block dev";
    if(S_ISFIFO(stat_buffer->st_mode)) return "fifo";
    if(S_ISLNK(stat_buffer->st_mode)) return "slink";
    if(S_ISSOCK(stat_buffer->st_mode)) return "sock";
    return "unknown";
}

static int fn(const char* path, const struct stat* stat, int flag, struct FTW* ftw)
{
    count++;
    if(count == 1) return 0;

    int print = 1;
    if(sign_ == -1)
        if(stat->st_mtime/60 >= time_/60) print=0;
    if(sign_ == 0)
        if(stat->st_mtime/60 != time_/60) print=0;
    if(sign_ == 1)
        if(stat->st_mtime/60 <= time_/60) print=0;


    if(print == 1) {
        struct tm *atime = localtime(&stat->st_atime);
        char atime_buffer[50];
        strftime (atime_buffer,50,"%Y-%m-%d %H:%M",atime);

        struct tm *mtime = localtime(&stat->st_mtime);
        char mtime_buffer[50];
        strftime (mtime_buffer,50,"%Y-%m-%d %H:%M",mtime);

        printf("%10s %11ldB %16s %16s %s\n", get_tile_type(stat),
                stat->st_size, atime_buffer, mtime_buffer, path);


    }

    return 0;
}

int view_dir(char *path, int sign_s, time_t time_s) 
{
    count = 0;
    time_ = time_s;
    sign_ = sign_s;
    if(sign_s < -1 || sign_s > 1)
        return create_error("Wrong sign");

    if (nftw(path, fn, 1000000, FTW_PHYS) == -1) {
        return create_error("nftw error");
    }

    return 0;
}

