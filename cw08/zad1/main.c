#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "image.h"
#include "filter.h"

int main()
{
    img_t image;
    img_load("images/lena.ascii.pgm", &image);

    img_t image_out;
    img_new(&image_out, &image);

    filter_t filter;
    filter_load("images/filter4.txt", &filter);
    filter_normalize(&filter);

    for (int y = 0; y < image.height; y++)
        for (int x = 0; x < image.width; x++)
            filter_apply(&filter, &image, &image_out, x, y, EDGE_WRAP);

    img_save("images/lena2.ascii.pgm", &image_out);

    return 0;
}