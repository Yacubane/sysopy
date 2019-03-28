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

static int copies_num = 0;
static int loop_enabled = 0;

static int create_error(char *message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}
static char* create_error_null(char *message) 
{
    fprintf(stderr, "%s\n", message);
    return NULL;
}
char* load_file(const char *path)
{
    FILE* fd;
    if ((fd = fopen(path, "r")) == NULL)
        return create_error_null("Cannot read file");

    int size;
    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    char *file_buffer = calloc(size, sizeof(char));
    if (fread(file_buffer, 1, size, fd) < 0) {
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
    if ((fd = fopen(path, "w+")) == NULL)
        return -1;
    
    int length = strlen(file_content);
    if (fwrite(file_content, 1, length, fd) != length) {
        fclose(fd);
        return -1;
    }
    fclose(fd);
    return 0;
}


void handleSIGTERM(int signum){
    exit(copies_num);
}

void handleSIGUSR1(int signum){
    loop_enabled = 0;
}

void handleSIGUSR2(int signum){
    loop_enabled = 1;
}

int fork_job(char* path, int refresh_seconds)
{
    int elapsed_seconds = 0;
    copies_num = 0;
    loop_enabled = 1;

    signal(SIGTERM, handleSIGTERM);
    signal(SIGUSR1, handleSIGUSR1);
    signal(SIGUSR2, handleSIGUSR2);

    struct stat stat_buffer;
    stat(path, &stat_buffer);
    unsigned long last_mod_time = stat_buffer.st_mtime;
    char *file_content = load_file(path);

    while (1) {
        while(loop_enabled) {
            sleep(refresh_seconds);
            elapsed_seconds += refresh_seconds;

            stat(path, &stat_buffer);
            if (last_mod_time != stat_buffer.st_mtime) {
                if (save_file(basename(path), file_content, &stat_buffer) < 0)
                    return create_error("Cannot save to file");
                copies_num++;
                free(file_content);
                file_content = load_file(path);
            }

            last_mod_time = stat_buffer.st_mtime;
        }
    }
    free(file_content);

    return copies_num;
}