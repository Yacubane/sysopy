#include <stdio.h>
#include "colors.h"
void set_color(char *color)
{
    printf("%s", color);
}
void reset_color()
{
    printf("%s\n", ANSI_COLOR_RESET);
}
void colorprint(char *buff, char *color)
{
    set_color(color);
    printf("%s", buff);
    reset_color();
}