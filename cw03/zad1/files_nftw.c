#define _XOPEN_SOURCE 500
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "files.h"

static int count;
static int path_size;

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}

static char* get_tile_type(const struct stat* stat_buffer) {
  if (S_ISREG(stat_buffer->st_mode))
    return "file";
  if (S_ISDIR(stat_buffer->st_mode))
    return "dir";
  if (S_ISCHR(stat_buffer->st_mode))
    return "char dev";
  if (S_ISBLK(stat_buffer->st_mode))
    return "block dev";
  if (S_ISFIFO(stat_buffer->st_mode))
    return "fifo";
  if (S_ISLNK(stat_buffer->st_mode))
    return "slink";
  if (S_ISSOCK(stat_buffer->st_mode))
    return "sock";
  return "unknown";
}
void make_fork(const char* path) {
  pid_t child_pid = fork();
  if (child_pid == 0) {
    execlp("ls", "ls", "-l", path, NULL);
  } else if (child_pid > 0) {
    printf("Child pid: %d, ls'ing dir: %s\n", child_pid, path + path_size);
    int status;
    waitpid(child_pid, &status, 0);
    printf("Child pid: %d ended\n", child_pid);
  } else {
    fprintf(stderr, "Cannot make fork\n");
  }
}
static int fn(const char* path,
              const struct stat* stat,
              int flag,
              struct FTW* ftw) {
  count++;
  if (count == 1)
    return 0;

  if (flag == FTW_D) {
    make_fork(path);
  }

  return 0;
}

int view_dir(char* path) {
  char* abosolute_path = malloc(1024 * sizeof(char));

  if (!realpath(path, abosolute_path)) {
    free(abosolute_path);
    return create_error("Cannot get real path");
  }

  count = 0;
  path_size = strlen(abosolute_path);

  if (nftw(abosolute_path, fn, 1000000, FTW_PHYS) == -1) {
    free(abosolute_path);
    return create_error("nftw error");
  }

  free(abosolute_path);
  return 0;
}
