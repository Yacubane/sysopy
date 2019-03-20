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
#include <sys/wait.h>

static time_t time_;
static int sign_;
static int count;
static int path_size;

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
void make_fork(const char *path) 
{
    pid_t child_pid = fork();
    if(child_pid == 0) 
    {
        execlp("ls", "ls", "-l", path, NULL);
    } else if(child_pid > 0)
    {
        printf("Child pid: %d, ls'ing dir: %s\n", child_pid, path + path_size);
        int status;
        waitpid(child_pid, &status, 0);
        printf("Child pid: %d ended\n", child_pid);
    } else 
    {
        fprintf(stderr, "Cannot make fork\n");
    }
}
static int fn(const char* path, const struct stat* stat, int flag, struct FTW* ftw)
{
    count++;
    if(count == 1) return 0;

    if(flag == FTW_D)
    {
        make_fork(path);
    }

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
    char* abosolute_path = malloc(1024*sizeof(char));

    if (!realpath(path, abosolute_path)) 
    {
        free(abosolute_path);
        return create_error("Cannot get real path");
    }

    count = 0;
    time_ = time_s;
    sign_ = sign_s;
    path_size = strlen(abosolute_path);
    if(sign_s < -1 || sign_s > 1)
    {
        free(abosolute_path);
        return create_error("Wrong sign");
    }

    if (nftw(abosolute_path, fn, 1000000, FTW_PHYS) == -1) 
    {
        free(abosolute_path);
        return create_error("nftw error");
    }

    free(abosolute_path);
    return 0;
}

