#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "conveyor_belt.h"
#include "errors.h"
#include "box.h"
#include "utils.h"
#include "shared.h"
#include "queue.h"
#include "config.h"
#include "colors.h"

int semid;
conveyor_belt_t *cb;

int cycles = 0;
int weight;
int last_res;

int pid;

void onexit()
{
    sha_clsem(semid);
    sha_unmapsha(cb, sizeof(conveyor_belt_t));
    set_color(ANSI_COLOR_CYAN);
    printf("Loader with PID: %d cleaned!", pid);
    reset_color();
}

void handle_sigint(int sig)
{
    exit(0);
}

void put_box()
{
    box_t box = create_box(weight);
    int res = cb_put(cb, semid, &box);
    if (res == 0)
    {
        set_color(ANSI_COLOR_GREEN);
        long timestamp = box.timestamp.tv_usec + (box.timestamp.tv_sec * 1000000);

        printf("Loader PID: %7d LOADED: %3d TIMESTAMP: %ld ",
               pid, weight, timestamp);
        reset_color();
    }

    else if (res == -1 && last_res != -1)
    {
        set_color(ANSI_COLOR_RED);
        printf("Loader PID: %7d - conveyor belt is full", pid);
        reset_color();
    }
    else if (res == -2 && last_res != -2)
    {
        set_color(ANSI_COLOR_YELLOW);
        printf("Loader PID: %7d - conveyor belt cannot take more weight", pid);
        reset_color();
    }
    else if (res == -3) //semaphore error
        exit(-1);
    else if (res == -4) //trucker closed
        exit(-1);

    last_res = res;
}

int main(int argc, char *argv[])
{
    pid = getpid();

    if (argc == 2)
    {
        if (parse_num(argv[1], &weight) < 0)
            return -1;
    }
    else if (argc == 3)
    {
        if (parse_num(argv[1], &weight) < 0)
            return -1;
        if (parse_num(argv[2], &cycles) < 0)
            return -1;
    }
    else
    {
        return err("Missing parametrs: weight cycles\n", -1);
    }

    int mem_id = sha_opnsha(get_shamem_key(), sizeof(conveyor_belt_t));
    cb = sha_mapsha(mem_id, sizeof(conveyor_belt_t));
    semid = sha_opnsem(cb->semaphore_key);

    atexit(onexit);
    signal(SIGINT, handle_sigint);

    if (cycles == 0)
        while (1)
        {
            put_box();
            usleep(LOADER_GET_NEXT_BOX_TIME);
        }

    else
        for (int i = 0; i < cycles; ++i)
        {
            put_box();
            usleep(LOADER_GET_NEXT_BOX_TIME);
        }

    return 0;
}