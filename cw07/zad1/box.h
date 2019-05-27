#pragma once
#include <sys/time.h>
#include "box.h"

typedef struct box_t {
  int pid;
  struct timeval timestamp;
  int weight;
} box_t;

box_t create_box(int weight);
