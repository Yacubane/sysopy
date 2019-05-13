#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return err("Missing parameters: num_workers max_weight", -1);
    }

    int num_workers;
    int max_weight;

    if (parse_num(argv[1], &num_workers) < 0)
        return -1;
    if (parse_num(argv[2], &max_weight) < 0)
        return -1;

    srand(time(NULL));

    char weight_string[10];
    for (int i = 0; i < num_workers; ++i)
    {
        sprintf(weight_string, "%d", (rand() % max_weight) + 1);

        if (fork() == 0)
        {
            execl("./loader", "./loader", weight_string, NULL);
            return -1;
        }
    }

    int status;
    for (int i = 0; i < num_workers; i++)
    {
        wait(&status);
        printf("Loader with PID %d finished with exit code %d\n", i, WEXITSTATUS(status));
    }

    return 0;
}