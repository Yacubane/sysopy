#define _GNU_SOURCE
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "file_monitor.h"

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}
static int create_error_and_close(FILE* file, char* message) {
  fclose(file);
  return create_error(message);
}

pid_t create_fork(char* path,
                  int refresh_seconds,
                  int monitor_seconds,
                  int type) {
  pid_t child_pid = fork();
  if (child_pid == 0) {
    exit(fork_job(path, refresh_seconds, monitor_seconds, type));
  } else if (child_pid > 0) {
    return child_pid;
  } else {
    return -1;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 4)
    return create_error("Please provide 3 arguments");

  FILE* fd;
  if ((fd = fopen(argv[1], "r")) == NULL)
    return create_error_and_close(fd, "Cannot open list file");

  errno = 0;
  char* end = NULL;
  int monitor_seconds = (int)strtol(argv[2], &end, 10);
  if (errno != 0 || end == argv[2])
    return create_error("Second argument is not a number");

  int type = 0;
  if (strcmp(argv[3], "buffer") == 0)
    type = 0;
  else if (strcmp(argv[3], "lazy") == 0)
    type = 1;
  else
    return create_error("Third argument must be \"buffer\" or \"lazy\"");

  system("mkdir -p archiwum");

  char path_buffer[255];
  int seconds_buffer;

  int iterator = 0;

  int pidSize = 0;
  while (fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) {
    pidSize++;
  }
  rewind(fd);

  pid_t pids[pidSize];

  while (fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) {
    pid_t child =
        create_fork(path_buffer, seconds_buffer, monitor_seconds, type);
    if (child < 0)
      return create_error("Cannot make fork");

    pids[iterator++] = child;
  }

  for (int i = 0; i < iterator; i++) {
    int status;
    waitpid(pids[i], &status, 0);
    printf("Proces %d utworzył %d kopii pliku\n", pids[i], WEXITSTATUS(status));
  }
}