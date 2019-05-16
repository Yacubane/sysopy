#pragma once
#include "image.h"


#define EDGE_WRAP (edge_mode_t) 1

typedef int edge_mode_t;


typedef struct filter_t
{
    float *array;
    int size;
    int normalized;
} filter_t;

int filter_load(const char *path, filter_t *out);
int filter_apply(filter_t *filter, img_t *image_in, img_t *image_out, int x, int y, edge_mode_t edge_mode);
int filter_normalize(filter_t *filter);