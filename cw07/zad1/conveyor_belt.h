#pragma once
#include "cirque_memblock.h"
#include "box.h"

typedef struct conveyor_belt_t
{
    cirque_mb_t queue;
    int max_weight;
    int semaphore_key;
} conveyor_belt_t;

int cb_put(conveyor_belt_t *cb, int semid, box_t *box);
int cb_get(conveyor_belt_t *cb, int semid, box_t *box);
int cb_peek(conveyor_belt_t *cb, int semid, box_t *box);
