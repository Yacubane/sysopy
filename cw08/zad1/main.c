#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "errors.h"
#include "utils.h"
#include "image.h"
#include "filter.h"

#define THREADS_TYPE_BLOCK 0
#define THREADS_TYPE_INTERLEAVED 1

static int threads_num;
static int threads_type;
static char *image_path;
static char *filter_path;
static char *out_image_path;
img_t image;
img_t out_image;
filter_t filter;

void *thread_fun(void *ptr)
{
    int *thread_num = (int *)ptr;
    if (threads_type == THREADS_TYPE_BLOCK)
    {
        int left_edge_x = *thread_num * (image.width / (float)threads_num);
        int right_edge_x = (*thread_num + 1) * (image.width / (float)threads_num);
        for (int x = left_edge_x; x < right_edge_x; x++)
            for (int y = 0; y < image.height; y++)
                filter_apply(&filter, &image, &out_image, x, y, EDGE_WRAP);
    }
    else if (threads_type == THREADS_TYPE_INTERLEAVED)
    {
        int x = *thread_num;
        while (x < image.width)
        {
            for (int y = 0; y < image.height; y++)
                filter_apply(&filter, &image, &out_image, x, y, EDGE_WRAP);
            x += threads_num;
        }
    }

    return (void *)911;
}

int main(int argc, char *argv[])
{

    if (argc != 6)
        return err("Missing arguments threads_num, thread_type, image_path, filter_path, out_image_path", -1);

    if (parse_int(argv[1], &threads_num) < 0)
        return -1;
    if (parse_int(argv[2], &threads_type) < 0)
        return -1;

    image_path = argv[3];
    filter_path = argv[4];
    out_image_path = argv[5];

    img_load(image_path, &image);
    img_new(&out_image, &image);

    filter_load(filter_path, &filter);

    pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);
    int *indexes = malloc(sizeof(int) * threads_num);

    for (int i = 0; i < threads_num; i++)
    {
        indexes[i] = i;
        pthread_create(&threads[i], NULL, thread_fun, &indexes[i]);
    }

    for (int i = 0; i < threads_num; i++)
        pthread_join(threads[i], NULL);

    img_save(out_image_path, &out_image);

    return 0;
}