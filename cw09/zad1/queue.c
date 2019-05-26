#include "queue.h"
#include <stdlib.h>
int quenew(queue_t *queue, int capacity)
{
    queue->size = 0;
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = capacity - 1;
    queue->array = malloc(sizeof(void *) * capacity);
    return 1;
}

int queclear(queue_t *queue)
{
    free(queue->array);
    return 0;
}

int queadd(queue_t *queue, void *item)
{

    int result;

    if (queue->size == queue->capacity)
        return -1;
    else
    {
        queue->tail = (queue->tail + 1) % queue->capacity;
        queue->array[queue->tail] = item;
        queue->size += 1;
        result = 0;
    }

    return result;
}

int queget(queue_t *queue, void **item)
{

    if (queue->size == 0)
        return -1;
    else
    {
        *item = queue->array[queue->head];
        queue->head = (queue->head + 1) % queue->capacity;
        queue->size -= 1;
    }

    return 0;
}
int quepeek(queue_t *queue, void **item)
{
    if (queue->size == 0)
        return -1;
    else
        *item = queue->array[queue->head];
    return 0;
}