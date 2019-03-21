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

static char* create_error_null(char *message) 
{
    fprintf(stderr, "%s\n", message);
    return NULL;
}
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

char* load_file(const char *path)
{
    FILE* fd;
    if((fd = fopen(path, "r")) == NULL)
        return create_error_null("Cannot read file");

    int size;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    char *file_buffer = calloc(size, sizeof(char));
    if(fread(file_buffer, 1, size, fd) < 0){
        fclose(fd);
        return create_error_null("Cannot read file");
    }
    fclose(fd);
    return file_buffer;
}
int save_file(const char *name, const char *file_content, struct stat *stat_buffer)
{
    struct tm *mtime = localtime(&stat_buffer->st_mtime);
    char mtime_buffer[50];
    strftime (mtime_buffer,50,"%Y-%m-%d_%H-%M-%S",mtime);

    char path[255];
    snprintf(path, 255, "%s/%s_%s", "archiwum", name, mtime_buffer);

    FILE* fd;
    if((fd = fopen(path, "w+")) == NULL)
        return -1;
    
    int length = strlen(file_content);
    if(fwrite(file_content, 1, length, fd) != length)
    {
        fclose(fd);
        return -1;
    }
    fclose(fd);
    return 0;
}

int fork_job(char* path, int refresh_seconds, int monitor_seconds, int type)
{
    int elapsed_seconds = 0;
    int copies_num = 0;
    if(type == 0)
    {
        struct stat stat_buffer;
        stat(path, &stat_buffer);
        unsigned long last_mod_time = stat_buffer.st_mtime;
        char *file_content = load_file(path);

        while(elapsed_seconds < monitor_seconds)
        {
            sleep(refresh_seconds);
            elapsed_seconds += refresh_seconds;

            stat(path, &stat_buffer);
            if(last_mod_time != stat_buffer.st_mtime)
            {
                if(save_file(basename(path), file_content, &stat_buffer) < 0)
                    return create_error("Cannot save to file");
                copies_num++;
                free(file_content);
                file_content = load_file(path);
            }

            last_mod_time = stat_buffer.st_mtime;
        }
        free(file_content);

        return copies_num;
    }
    else if (type == 1)
    {
        struct stat stat_buffer;
        stat(path, &stat_buffer);
        unsigned long last_mod_time = stat_buffer.st_mtime;

        while(elapsed_seconds < monitor_seconds)
        {
            sleep(refresh_seconds);
            elapsed_seconds += refresh_seconds;

            stat(path, &stat_buffer);
            if(last_mod_time != stat_buffer.st_mtime)
            {
                pid_t child_pid = fork();
                copies_num++;
                if(child_pid == 0) 
                {
                    struct tm *mtime = localtime(&stat_buffer.st_mtime);
                    char mtime_buffer[50];
                    strftime (mtime_buffer,50,"%Y-%m-%d_%H-%M-%S",mtime);

                    char new_path[255];
                    snprintf(new_path, 255, "%s/%s_%s", "archiwum", basename(path), mtime_buffer);

                    execlp("cp", "cp", path, new_path, NULL);
                    exit(0);
                } else if(child_pid > 0)
                {

                } else 
                {
                    return -1;
                } 
            }

            last_mod_time = stat_buffer.st_mtime;
        }

        return copies_num;
    }
    return 0;
}

pid_t create_fork(char* path, int refresh_seconds, int monitor_seconds, int type)
{
    pid_t child_pid = fork();
    if(child_pid == 0) 
    {
        exit(fork_job(path, refresh_seconds, monitor_seconds, type));
    } else if(child_pid > 0)
    {
        return child_pid;
    } else 
    {
        return -1;
    }
}



int main (int argc, char *argv[]) 
{
    if(argc < 4)
        return create_error("Please provite 3 arguments");

    FILE* fd;
    if((fd = fopen(argv[1], "r")) == NULL)
        return create_error_and_close(fd, "Cannot open list file");

    errno = 0;
    char* end = NULL;
    int monitor_seconds = (int) strtol(argv[2], &end, 10);
    if(errno != 0 || end == argv[2])
        return create_error("Second argument is not a number"); 


    int type = 0;
    if(strcmp(argv[3], "buffer") == 0)
        type = 0;
    else if(strcmp(argv[3], "lazy") == 0)
        type = 1;
    else return create_error("Third argument must be \"buffer\" or \"lazy\"");

    system("mkdir -p archiwum");

    char path_buffer[255];
    int seconds_buffer;

    int iterator = 0;
    pid_t pids[255]; 
    #warning 255 might not be good xD

    while(fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) 
    {    
        pid_t child = create_fork(path_buffer, seconds_buffer, monitor_seconds, type);
        if(child < 0)
            return create_error("Cannot make fork");
        
        pids[iterator++] = child;
    }
    
    for (int i  = 0; i < iterator; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        printf("Proces %d utworzył %d kopii pliku\n", pids[i], WEXITSTATUS(status));
    }
}