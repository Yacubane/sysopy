#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include <time.h>
#include <string.h>
#include <sys/resource.h>
#include "file_monitor.h"

static int create_error(char *message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}
static int create_error_and_close(FILE *file, char *message) 
{
    fclose(file);
    return create_error(message);
}

pid_t create_fork(char* path, int refresh_seconds, int monitor_seconds, int type,
    int cpu_restriction, int memory_restriction)
{
    pid_t child_pid = fork();
    if (child_pid == 0) {
        exit(fork_job(path, refresh_seconds, monitor_seconds, type, cpu_restriction, memory_restriction));
    } else if(child_pid > 0) {
        return child_pid;
    } else {
        return -1;
    }
}



int main (int argc, char *argv[]) 
{
    if(argc < 6)
        return create_error("Please provide 5 arguments");

    FILE* fd;
    if ((fd = fopen(argv[1], "r")) == NULL)
        return create_error_and_close(fd, "Cannot open list file");

    char* end;

    errno = 0;
    end = NULL;
    int monitor_seconds = (int) strtol(argv[2], &end, 10);
    if (errno != 0 || end == argv[2])
        return create_error("Second argument is not a number"); 


    int type = 0;
    if (strcmp(argv[3], "buffer") == 0)
        type = 0;
    else if (strcmp(argv[3], "lazy") == 0)
        type = 1;
    else return create_error("Third argument must be \"buffer\" or \"lazy\"");

    errno = 0;
    end = NULL;
    int cpu_restriction = (int) strtol(argv[4], &end, 10);
    if (errno != 0 || end == argv[4])
        return create_error("Fourth argument is not a number"); 

    errno = 0;
    end = NULL;
    int memory_restriction = (int) strtol(argv[5], &end, 10);
    if (errno != 0 || end == argv[5])
        return create_error("Fifth argument is not a number"); 

    system("mkdir -p archiwum");

    char path_buffer[255];
    int seconds_buffer;
    
    while (fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) {    
        pid_t child = create_fork(path_buffer, seconds_buffer, monitor_seconds, type,
            cpu_restriction, memory_restriction);
        if(child < 0)
            return create_error("Cannot make fork");
    }
    
    errno = 0;
    while (1) {
        int status;
        int pid = wait(&status);
        if (pid != -1) {
            if (WIFEXITED(status))
                printf("Proces %d utworzył %d kopii pliku\n",
                pid, WEXITSTATUS(status));
            else if (WIFSIGNALED(status))
                printf("Proces %d zakończony sygnałem %d %s\n",
                pid, WTERMSIG(status), strsignal(WTERMSIG(status)));
        }
        if (pid == -1 && errno == ECHILD)
            break;
    }

}