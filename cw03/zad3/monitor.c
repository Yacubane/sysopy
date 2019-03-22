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
static int cpu_restriction;
static int memory_restriction;

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

int fork_job(char* path, int refresh_seconds, int monitor_seconds, int type)
{
    struct rlimit cpu_rlimit, memory_rlimit;

    cpu_rlimit.rlim_cur=cpu_restriction;
    cpu_rlimit.rlim_max=cpu_restriction;

    memory_rlimit.rlim_cur=memory_restriction*1024*1024;
    memory_rlimit.rlim_max=memory_restriction*1024*1024;

    if(setrlimit(RLIMIT_CPU, &cpu_rlimit) < 0)
        return create_error("Cannot assaign CPU limit"); 
    if(setrlimit(RLIMIT_AS, &memory_rlimit) < 0)
        return create_error("Cannot assaign memory limit"); 

    int elapsed_seconds = 0;
    int copies_num = 0;
    if (type == 0) {
        struct stat stat_buffer;
        stat(path, &stat_buffer);
        unsigned long last_mod_time = stat_buffer.st_mtime;
        char *file_content = load_file(path);

        while (elapsed_seconds < monitor_seconds) {
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
        free(file_content);
        struct rusage rusage;
        getrusage(RUSAGE_SELF, &rusage);
        printf("Raport podprocesu o PID: %d. u_time: %lu [s] %lu [us] s_time: %lu [s] %ld [us] maxrss %ld\n",
            getpid(), rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec,
            rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec,
            rusage.ru_maxrss);
    }
    else if (type == 1) {
        struct stat stat_buffer;
        stat(path, &stat_buffer);
        unsigned long last_mod_time = stat_buffer.st_mtime;

        while (elapsed_seconds < monitor_seconds) {
            sleep(refresh_seconds);
            elapsed_seconds += refresh_seconds;

            stat(path, &stat_buffer);
            if (last_mod_time != stat_buffer.st_mtime) {
                pid_t child_pid = fork();
                copies_num++;
                if (child_pid == 0) {
                    struct tm *mtime = localtime(&stat_buffer.st_mtime);
                    char mtime_buffer[50];
                    strftime (mtime_buffer,50,"%Y-%m-%d_%H-%M-%S",mtime);

                    char new_path[255];
                    snprintf(new_path, 255, "%s/%s_%s", "archiwum", basename(path), mtime_buffer);

                    execlp("cp", "cp", path, new_path, NULL);
                    exit(0);
                } else if(child_pid > 0) {

                } else {
                    return -1;
                } 
            }

            last_mod_time = stat_buffer.st_mtime;
        }
        struct rusage rusage;
        getrusage(RUSAGE_SELF, &rusage);
        printf("Raport podprocesu o PID: %d. u_time: %lu [s] %lu [us] s_time: %lu [s] %ld [us] maxrss %ld\n",
            getpid(), rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec,
            rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec,
            rusage.ru_maxrss);

        while (1) {
            int status;
            int pid = wait(&status);
            if (pid == -1 && errno == ECHILD)
                break;
        }
        
        getrusage(RUSAGE_CHILDREN, &rusage);
        printf("Raport podprocesu dzieci o PID: %d. u_time: %lu [s] %lu [us] s_time: %lu [s] %ld [us] maxrss %ld\n",
            getpid(), rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec,
            rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec,
            rusage.ru_maxrss);
    }
   
    return copies_num;
}

pid_t create_fork(char* path, int refresh_seconds, int monitor_seconds, int type)
{
    pid_t child_pid = fork();
    if (child_pid == 0) {
        exit(fork_job(path, refresh_seconds, monitor_seconds, type));
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
    int cpu_restriction_l = (int) strtol(argv[4], &end, 10);
    if (errno != 0 || end == argv[4])
        return create_error("Fourth argument is not a number"); 
    cpu_restriction = cpu_restriction_l;

    errno = 0;
    end = NULL;
    int memory_restriction_l = (int) strtol(argv[5], &end, 10);
    if (errno != 0 || end == argv[5])
        return create_error("Fifth argument is not a number"); 
    memory_restriction = memory_restriction_l;

    system("mkdir -p archiwum");

    char path_buffer[255];
    int seconds_buffer;
    
    while (fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) {    
        pid_t child = create_fork(path_buffer, seconds_buffer, monitor_seconds, type);
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