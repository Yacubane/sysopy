#include "conveyor_belt.h"
#include "shared.h"
#include "queue.h"
#include "box.h"
#include <stdio.h>

int cb_put(conveyor_belt_t *cb, int semid, box_t *box)
{
    int to_return = 0;
    if (sha_lcksem(semid) < 0)
        return -3;

    box_t tmp_box;
    int weight = 0;
    for (int i = 0; i < que_size(&cb->queue); i++)
    {
        que_peek(&cb->queue, i, &tmp_box);
        weight += tmp_box.weight;
    }

    if (weight + box->weight > cb->max_weight)
        to_return = -1;
    else if (que_put(&cb->queue, box) < 0)
        to_return = -2;

    if (sha_unlcksem(semid) < 0)
        return -3;
    return to_return;
}
int cb_get(conveyor_belt_t *cb, int semid, box_t *box)
{
    int to_return = 0;
    sha_lcksem(semid);
    if (que_get(&cb->queue, box) < 0)
    {
        box = NULL;
        to_return = -1;
    }
    sha_unlcksem(semid);
    return to_return;
}
int cb_peek(conveyor_belt_t *cb, int semid, box_t *box)
{
    int to_return = 0;
    sha_lcksem(semid);
    if (que_size(&cb->queue) == 0)
        to_return = -1;
    else if (que_peek(&cb->queue, 0, box) < 0)
    {
        box = NULL;
        to_return = -1;
    }

    sha_unlcksem(semid);
    return to_return;
}