#pragma once
#define MAX_CAPACITY 512

typedef struct cirque_mb_t
{
    int actual_size;

    int item_size;
    int max_size;

    int head;
    int tail;

    char array[MAX_CAPACITY];
} cirque_mb_t;