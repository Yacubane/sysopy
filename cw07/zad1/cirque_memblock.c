#include "errors.h"
#include "queue.h"
#include "cirque_memblock.h"
#include <string.h>
#include <stdlib.h>

void *que_create(int item_size, int max_size)
{
    if (item_size * max_size > MAX_CAPACITY)
    {
        return NULL;
    }

    cirque_mb_t *queue = malloc(sizeof(cirque_mb_t));
    que_clear(queue, item_size, max_size);
    return queue;
}
int que_clear(void *queue_ptr, int item_size, int max_size)
{
    cirque_mb_t *queue = (cirque_mb_t *)queue_ptr;

    if (item_size * max_size > MAX_CAPACITY)
        return -1;
    queue->actual_size = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->item_size = item_size;
    queue->max_size = max_size;
    return 0;
}
int que_put(void *queue_ptr, void *item_ptr)
{
    cirque_mb_t *queue = (cirque_mb_t *)queue_ptr;

    if (queue->actual_size == queue->max_size)
    {
        return -1;
    }
    else
    {
        int elem_num = queue->tail;
        queue->tail = (queue->tail + 1) % queue->max_size;
        memcpy(queue->array + elem_num * queue->item_size, item_ptr, queue->item_size);
        queue->actual_size += 1;
        return 0;
    }
}
int que_get(void *queue_ptr, void *item_ptr)
{
    cirque_mb_t *queue = (cirque_mb_t *)queue_ptr;

    if (queue->actual_size == 0)
    {
        return -1;
    }
    else
    {
        memcpy(item_ptr, queue->array + queue->head * queue->item_size, queue->item_size);
        queue->head = (queue->head + 1) % queue->max_size;
        queue->actual_size -= 1;
    }
    return 0;
}
int que_size(void *queue_ptr)
{
    cirque_mb_t *queue = (cirque_mb_t *)queue_ptr;
    return queue->actual_size;
}
int que_peek(void *queue_ptr, int elem_num, void *item_ptr)
{
    cirque_mb_t *queue = (cirque_mb_t *)queue_ptr;

    if (elem_num >= queue->actual_size)
    {
        return -1;
    }
    else
    {
        elem_num = (queue->head + elem_num) % queue->max_size;
        memcpy(item_ptr, queue->array + elem_num * queue->item_size, queue->item_size);
    }
    return 0;
}