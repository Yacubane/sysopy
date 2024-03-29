#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "box.h"
#include "colors.h"
#include "config.h"
#include "conveyor_belt.h"
#include "errors.h"
#include "queue.h"
#include "shared.h"
#include "utils.h"

conveyor_belt_t* cb;
int semid;
int memid;

void onexit() {
  sha_lcksem(semid);
  cb->is_trucker = 0;
  printf("Trucker locked semaphore last time\n");
  sha_rmsem(get_semaphore_key(), semid);
  sha_unmapsha(cb, sizeof(conveyor_belt_t));
  sha_rmsha(get_shamem_key(), memid);
  printf("Trucker cleaned...\n");
}

void handleSIGINT(int sig) {
  exit(0);
}

struct timeval timeval_diff(struct timeval start, struct timeval stop) {
  struct timeval temp;
  if ((stop.tv_usec - start.tv_usec) < 0) {
    temp.tv_sec = stop.tv_sec - start.tv_sec - 1;
    temp.tv_usec = 1000000 + stop.tv_usec - start.tv_usec;
  } else {
    temp.tv_sec = stop.tv_sec - start.tv_sec;
    temp.tv_usec = stop.tv_usec - start.tv_usec;
  }
  return temp;
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    return err("missing parameters: trucker_capacity cb_size cb_capacity\n",
               -1);
  }

  atexit(onexit);
  signal(SIGINT, handleSIGINT);

  int trucker_capacity;
  int cb_size;
  int cb_capacity;

  if (parse_num(argv[1], &trucker_capacity) < 0)
    return -1;
  if (parse_num(argv[2], &cb_size) < 0)
    return -1;
  if (parse_num(argv[3], &cb_capacity) < 0)
    return -1;

  int actual_weight = 0;

  memid = sha_opnsha(get_shamem_key(), sizeof(conveyor_belt_t));
  sha_rmsha(get_shamem_key(), memid);

  memid = sha_newsha(get_shamem_key(), sizeof(conveyor_belt_t));

  cb = sha_mapsha(memid, sizeof(conveyor_belt_t));
  cb->max_weight = cb_capacity;
  cb->semaphore_key = get_semaphore_key();
  cb->is_trucker = 1;

  semid = sha_opnsem(get_semaphore_key());
  sha_rmsem(get_semaphore_key(), semid);

  semid = sha_newsem(get_semaphore_key());
  if (que_clear(&cb->queue, sizeof(box_t), cb_size) == -1)
    return err("Too big size for queue", -1);

  box_t box;
  struct timeval timestamp;

  printf("Empty truck come\n");

  int was_empty = 0;

  while (1) {
    if (cb_get(cb, semid, &box) < 0) {
      if (was_empty == 0)
        printf("Conveyor belt empty\n");

      was_empty = 1;
      continue;
    }
    was_empty = 0;

    gettimeofday(&timestamp, NULL);

    if (trucker_capacity < actual_weight + box.weight) {
      printf("Full truck - offloading\n");
      sha_lcksem(semid);
      actual_weight = 0;
      usleep(TRUCKER_OFFLOADING_TIME_MICROS);
      sha_unlcksem(semid);
      printf("Empty truck come\n");
    }

    if (box.weight > trucker_capacity)
      return err("Trucker capacity is too small", -1);

    usleep(TRUCKER_LOAD_BOX_TIME_MICROS);

    actual_weight += box.weight;

    struct timeval diff = timeval_diff(box.timestamp, timestamp);

    long elapsed = diff.tv_usec + (diff.tv_sec * 1000000);
    long timestamp_val = timestamp.tv_usec + (timestamp.tv_sec * 1000000);

    printf(
        "BOX PID: %7d TIME %7ldus WEIGHT %2d OCCUPIED %2d REMAIN %2d TIMESTAMP "
        "%ldus\n",
        box.pid, elapsed, box.weight, actual_weight,
        trucker_capacity - actual_weight, timestamp_val);
  }

  return 0;
}