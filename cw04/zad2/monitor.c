#include <errno.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "file_monitor.h"

typedef struct {
  pid_t pid;
  int status;

} monitor_t;

monitor_t** monitors;
int monitors_size;

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}
static int create_error_and_close(FILE* file, char* message) {
  fclose(file);
  return create_error(message);
}

pid_t create_fork(char* path, int refresh_seconds) {
  pid_t child_pid = fork();
  if (child_pid == 0) {
    exit(fork_job(path, refresh_seconds));
  } else if (child_pid > 0) {
    return child_pid;
  } else {
    return -1;
  }
}

int start_monitor(int pid) {
  for (int i = 0; i < monitors_size; i++) {
    if (monitors[i]->pid == pid) {
      kill(pid, SIGUSR2);
      monitors[i]->status = 1;
      printf("Started process %d\n", pid);
      return 0;
    }
  }
  printf(
      "Cannot start monitor with PID %d. Didn't found process with that PID\n",
      pid);
  return -1;
}
int stop_monitor(int pid) {
  for (int i = 0; i < monitors_size; i++) {
    if (monitors[i]->pid == pid) {
      kill(pid, SIGUSR1);
      monitors[i]->status = 0;
      printf("Stopped process %d\n", pid);
      return 0;
    }
  }
  printf(
      "Cannot stop monitor with PID %d. Didn't found process with that PID\n",
      pid);
  return -1;
}

void end() {
  for (int i = 0; i < monitors_size; i++) {
    kill(monitors[i]->pid, SIGTERM);
  }
  for (int i = 0; i < monitors_size; i++) {
    int status;
    waitpid(monitors[i]->pid, &status, 0);
    printf("Process %d created %d copies\n", monitors[i]->pid,
           WEXITSTATUS(status));
  }
  exit(0);
}

void handleSIGINT(int signum) {
  end();
}

int main(int argc, char* argv[]) {
  signal(SIGINT, handleSIGINT);

  if (argc != 2)
    return create_error("Please provide 1 argument");

  FILE* fd;
  if ((fd = fopen(argv[1], "r")) == NULL)
    return create_error_and_close(fd, "Cannot open list file");

  system("mkdir -p archiwum");

  char path_buffer[255];
  int seconds_buffer;

  int iterator = 0;

  monitors_size = 0;
  while (fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) {
    monitors_size++;
  }
  rewind(fd);

  monitors = calloc(monitors_size, sizeof(monitor_t*));

  while (fscanf(fd, "%s %d\n", path_buffer, &seconds_buffer) == 2) {
    pid_t child = create_fork(path_buffer, seconds_buffer);
    if (child < 0)
      return create_error("Cannot make fork");
    monitors[iterator] = malloc(sizeof(monitor_t));
    monitors[iterator]->status = 1;
    monitors[iterator++]->pid = child;

    printf("Created process with PID %d, which monitors %s\n", child,
           path_buffer);
  }

  char buffer[255];
  while (1) {
    fgets(buffer, 255, stdin);
    if (!strncmp(buffer, "LIST", 4)) {
      for (int i = 0; i < monitors_size; i++) {
        const char* const status0text = "OFF";
        const char* const status1text = "ON";

        const char* status_text = status0text;
        if (monitors[i]->status)
          status_text = status1text;

        printf("%d %s\n", monitors[i]->pid, status_text);
      }
    } else if (!strncmp(buffer, "STOP ALL", 8)) {
      for (int i = 0; i < monitors_size; i++) {
        stop_monitor(monitors[i]->pid);
      }
    } else if (!strncmp(buffer, "START ALL", 9)) {
      for (int i = 0; i < monitors_size; i++) {
        start_monitor(monitors[i]->pid);
      }
    } else if (!strncmp(buffer, "STOP", 4)) {
      char* numberBuffer = buffer + 5;
      int number = atoi(numberBuffer);
      stop_monitor(number);
    } else if (!strncmp(buffer, "START", 4)) {
      char* numberBuffer = buffer + 6;
      int number = atoi(numberBuffer);
      start_monitor(number);
    } else if (!strncmp(buffer, "END", 3)) {
      end();
    } else {
      printf("Unknown command\n");
    }
  }
}