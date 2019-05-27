#include "box.h"
#include <sys/time.h>
#include <unistd.h>

box_t create_box(int weight) {
  box_t box;
  box.pid = getpid();
  gettimeofday(&box.timestamp, NULL);
  box.weight = weight;
  return box;
}