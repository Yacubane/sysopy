#include <dirent.h>
#include "files.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
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
void make_fork(const char *rel_path_buff, const char *file_name) 
{
    pid_t child_pid = fork();
    if(child_pid == 0) 
    {
        execlp("ls", "ls", "-l", file_name, NULL);
    } else if(child_pid > 0)
    {
        printf("Child pid: %d, ls'ing dir: %s/%s\n", child_pid, rel_path_buff, file_name);
        int status;
        waitpid(child_pid, &status, 0);
        printf("Child pid: %d ended\n", child_pid);
    } else 
    {
        fprintf(stderr, "Cannot make fork\n");
    }
}

int readdir_recursive(struct stat *stat_buffer, int buffer_size, char *rel_path_buff) 
{
    DIR* dir;
    if((dir = opendir(".")) == NULL) return create_error("opendir error");


    struct dirent* file;
    errno = 0;
    while((file = readdir(dir)) != NULL) {
        if(strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
        {           
            if(lstat(file->d_name, stat_buffer) < 0) return create_error_and_close_dir(dir, "stat error");
            if(S_ISDIR(stat_buffer->st_mode)){

                make_fork(rel_path_buff, file->d_name);
                char rel_path_buff[512];
                snprintf(rel_path_buff, 512, "%s/%s", rel_path_buff, file->d_name);
                if(chdir(file->d_name) != 0) 
                    return create_error_and_close_dir(dir, "Cannot get forward in files tree");
                readdir_recursive(stat_buffer, buffer_size, rel_path_buff);
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


int view_dir(char *path) 
{
    struct stat *stat_buffer = malloc(sizeof(struct stat));
    if(chdir(path) != 0) return create_error("Cannot change directory to choosen path");
    readdir_recursive(stat_buffer, 255, "");
    free(stat_buffer);
    return 0;
}

