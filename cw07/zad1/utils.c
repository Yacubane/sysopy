#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <time.h>
#include "errors.h"

int parse_num(const char* string, int* out) {
  errno = 0;
  char* end = NULL;
  int tmp = (int)strtol(string, &end, 10);
  if (errno != 0 || end == string)
    return err("Parsing num failed", -1);
  *out = tmp;
  return 0;
}

int get_semaphore_key() {
  return ftok(getenv("HOME"), 911);
}

int get_shamem_key() {
  return ftok(getenv("HOME"), 2137);
}
