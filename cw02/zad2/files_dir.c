#include <dirent.h>
#include "files.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

static int create_error(char *message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}

static int create_error_and_close_dir(DIR *dir, char *message) 
{
    closedir(dir);
    return create_error(message);
}
static char* get_tile_type(struct stat *stat_buffer) 
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
int readdir_recursive(struct stat *stat_buffer, int buffer_size, int sign, time_t time) 
{
    DIR* dir;
    if((dir = opendir(".")) == NULL) return create_error("opendir error");


    struct dirent* file;
    errno = 0;
    while((file = readdir(dir)) != NULL) {
        if(strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
        {
            if(lstat(file->d_name, stat_buffer) < 0) return create_error_and_close_dir(dir, "stat error");
            int print = 1;
            if(sign == -1)
                if(stat_buffer->st_mtime/60 >= time/60) print=0;
            if(sign == 0)
                if(stat_buffer->st_mtime/60 != time/60) print=0;
            if(sign == 1)
                if(stat_buffer->st_mtime/60 <= time/60) print=0;


            if(print == 1) {
                struct tm *atime = localtime(&stat_buffer->st_atime);
                char atime_buffer[50];
                strftime (atime_buffer,50,"%Y-%m-%d %H:%M",atime);

                struct tm *mtime = localtime(&stat_buffer->st_mtime);
                char mtime_buffer[50];
                strftime (mtime_buffer,50,"%Y-%m-%d %H:%M",mtime);

                char cwd_buffer[255];
                if(getcwd(cwd_buffer, 255) == NULL) return create_error_and_close_dir(dir, "getcwd error");

                printf("%10s %11ldB %16s %16s %s/%s\n", get_tile_type(stat_buffer),
                stat_buffer->st_size, atime_buffer, mtime_buffer, cwd_buffer, file->d_name);
            }
            
            if(S_ISDIR(stat_buffer->st_mode)){
                if(chdir(file->d_name) != 0) 
                    return create_error_and_close_dir(dir, "Cannot get forward in files tree");
                readdir_recursive(stat_buffer, buffer_size, sign, time);
                if(chdir("..") != 0) 
                    return create_error_and_close_dir(dir, "Cannot get back in files tree");
            }
        }

    }
    if(errno != 0)
        return create_error("cannot read dir");
    errno = 0;
    closedir(dir);

   return 0;
}
int view_dir(char *path, int sign, time_t time) 
{
    if(sign < -1 || sign > 1)
        return create_error("Wrong sign");

    struct stat *stat_buffer = malloc(sizeof(struct stat));

    if(chdir(path) != 0) return create_error("Cannot change directory to choosen path");
    readdir_recursive(stat_buffer, 255, sign, time);
    free(stat_buffer);
    return 0;
}

