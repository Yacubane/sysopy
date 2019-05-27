#pragma once

#include <stddef.h>

typedef struct queue_t {
  void** array;
  int tail;
  int head;

  int size;
  int capacity;

} queue_t;

int quenew(queue_t* queue, int capacity);
int queadd(queue_t* queue, void* item);
int queget(queue_t* queue, void** item);
int quepeek(queue_t* queue, void** item);
int queclear(queue_t* queue);