#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "errors.h"
#include "utils.h"
#include "queue.h"
#include "colors.h"
#include <sys/time.h>

#define STATE_READY_TO_BOARD 0
#define STATE_BOARDING 1
#define STATE_IN_TROLLEY_WAITING_FOR_NOTHING 2
#define STATE_IN_TROLLEY_WAITING_TO_CLICK_RED 3
#define STATE_IN_TROLLEY_CLICKED_RED_BUTTON 4
#define STATE_IN_TROLLEY 5
#define STATE_READY_TO_RELEASE 6
#define STATE_RELEASED 7

#define TEXT_BUFF_SIZE 255

typedef struct trolley_t trolley_t;
typedef struct passenger_t passenger_t;

struct passenger_t
{
    int id;
    int state;
    trolley_t *trolley;
    pthread_t thread;
};

struct trolley_t
{
    int id;
    int state;
    pthread_t thread;
    int passengers_count;
    passenger_t **passengers;
};

static int passengers_num;
static int trolleys_num;
static int trolley_capacity;
static int rides_num;
static int riding_trolleys_num;

static passenger_t **passengers;
static trolley_t **trolleys;
static queue_t *passengers_queue;
static queue_t *trolleys_queue;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_add_passenger_to_queue = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_select_passenger_to_board = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_board = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_select_passenger_to_click = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_onboard_action = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_platform_queue_change = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_select_passenger_to_release = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_passenger_release = PTHREAD_COND_INITIALIZER;

static unsigned long long start_millis = 0;
char text_buff[TEXT_BUFF_SIZE];
char timestamp_text_buff[TEXT_BUFF_SIZE];

unsigned long long millis()
{
    unsigned long long res = 0;
    struct timeval timestamp;
    gettimeofday(&timestamp, NULL);
    res = timestamp.tv_sec * 1000 + timestamp.tv_usec / 1000;
    return res;
}
unsigned long long millis_since_start()
{
    return millis() - start_millis;
}

char *add_timestamp(char *buff)
{
    snprintf(timestamp_text_buff, TEXT_BUFF_SIZE,
             "TIME=%5llu %s", millis_since_start(), buff);
    return timestamp_text_buff;
}

void *passenger_thread_fun(void *data)
{
    passenger_t *passenger = (passenger_t *)data;
    pthread_mutex_lock(&mutex);

    while (1)
    {
        while (passenger->state != STATE_READY_TO_BOARD && riding_trolleys_num != 0)
            pthread_cond_wait(&cond_select_passenger_to_board, &mutex);

        if (riding_trolleys_num == 0)
            break;
        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "PASSENGER[%d] is boarding. Passengers in trolley: %d",
                 passenger->id, passenger->trolley->passengers_count + 1);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_MAGENTA);

        passenger->state = STATE_BOARDING;
        pthread_cond_broadcast(&cond_passenger_board);

        while (passenger->state != STATE_IN_TROLLEY_WAITING_FOR_NOTHING && passenger->state != STATE_IN_TROLLEY_WAITING_TO_CLICK_RED)
            pthread_cond_wait(&cond_select_passenger_to_click, &mutex);

        if (passenger->state == STATE_IN_TROLLEY_WAITING_TO_CLICK_RED)
        {
            snprintf(text_buff, TEXT_BUFF_SIZE,
                     "PASSENGER[%d] is clicking red button",
                     passenger->id);
            colorprint(add_timestamp(text_buff), ANSI_COLOR_RED);
            passenger->state = STATE_IN_TROLLEY_CLICKED_RED_BUTTON;
        }
        else if (passenger->state == STATE_IN_TROLLEY_WAITING_FOR_NOTHING)
            passenger->state = STATE_IN_TROLLEY;

        pthread_cond_broadcast(&cond_passenger_onboard_action);
        while (passenger->state != STATE_READY_TO_RELEASE)
            pthread_cond_wait(&cond_select_passenger_to_release, &mutex);
        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "PASSENGER[%d] is releasing. Passengers in trolley: %d",
                 passenger->id, passenger->trolley->passengers_count - 1);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_MAGENTA);
        passenger->state = STATE_RELEASED;
        pthread_cond_broadcast(&cond_passenger_release);
        queadd(passengers_queue, passenger);
    }
    snprintf(text_buff, TEXT_BUFF_SIZE,
             "PASSENGER[%d] is ending it's job", passenger->id);
    colorprint(add_timestamp(text_buff), ANSI_COLOR_MAGENTA);
    pthread_mutex_unlock(&mutex);
    return 0;
}
void *trolley_thread_fun(void *data)
{
    trolley_t *trolley = (trolley_t *)data;
    pthread_mutex_lock(&mutex);

    trolley_t *item;
    while (quepeek(trolleys_queue, (void **)&item) == 0 && item != trolley)
        pthread_cond_wait(&cond_platform_queue_change, &mutex);

    for (int i = 0; i < rides_num; i++)
    {

        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "TROLLEY[%d] is opening doors", trolley->id);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);
        while (trolley->passengers_count < trolley_capacity)
        {

            passenger_t *passenger;
            while (queget(passengers_queue, (void **)&passenger) == -1)
                pthread_cond_wait(&cond_add_passenger_to_queue, &mutex);

            passenger->state = STATE_READY_TO_BOARD;
            passenger->trolley = trolley;
            pthread_cond_broadcast(&cond_select_passenger_to_board);
            while (passenger->state != STATE_BOARDING)
                pthread_cond_wait(&cond_passenger_board, &mutex);

            trolley->passengers[trolley->passengers_count++] = passenger;
            passenger->state = STATE_IN_TROLLEY_WAITING_FOR_NOTHING;
        }
        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "TROLLEY[%d] is closing doors", trolley->id);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);
        int selected_passenger = rand() % trolley_capacity;
        trolley->passengers[selected_passenger]->state = STATE_IN_TROLLEY_WAITING_TO_CLICK_RED;

        for (int i = 0; i < trolley->passengers_count; i++)
        {
            pthread_cond_broadcast(&cond_select_passenger_to_click);
            if (i == selected_passenger)
                while (trolley->passengers[i]->state != STATE_IN_TROLLEY_CLICKED_RED_BUTTON)
                    pthread_cond_wait(&cond_passenger_onboard_action, &mutex);

            else
                while (trolley->passengers[i]->state != STATE_IN_TROLLEY)
                    pthread_cond_wait(&cond_passenger_onboard_action, &mutex);
        }

        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "TROLLEY[%d] is starting a ride", trolley->id);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);
        queget(trolleys_queue, (void **)&item);
        queadd(trolleys_queue, trolley);
        pthread_cond_broadcast(&cond_platform_queue_change);

        pthread_mutex_unlock(&mutex);
        usleep(1000 * 5);
        pthread_mutex_lock(&mutex);

        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "TROLLEY[%d] is ending a ride", trolley->id);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);

        while (quepeek(trolleys_queue, (void **)&item) == 0 && item != trolley)
            pthread_cond_wait(&cond_platform_queue_change, &mutex);

        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "TROLLEY[%d] is opening doors", trolley->id);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);
        while (trolley->passengers_count > 0)
        {
            trolley->passengers[trolley->passengers_count - 1]->state = STATE_READY_TO_RELEASE;
            pthread_cond_broadcast(&cond_select_passenger_to_release);

            while (trolley->passengers[trolley->passengers_count - 1]->state != STATE_RELEASED)
                pthread_cond_wait(&cond_passenger_release, &mutex);
            trolley->passengers[trolley->passengers_count - 1]->trolley = NULL;

            trolley->passengers_count--;
        }
        snprintf(text_buff, TEXT_BUFF_SIZE,
                 "TROLLEY[%d] is closing doors", trolley->id);
        colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);
    }
    riding_trolleys_num--;

    queget(trolleys_queue, (void **)&item);
    pthread_cond_broadcast(&cond_platform_queue_change);
    //wakeup passangers, notify that riding_trolleys_num changed
    pthread_cond_broadcast(&cond_select_passenger_to_board);
    snprintf(text_buff, TEXT_BUFF_SIZE,
             "TROLLEY[%d] is ending it's job", trolley->id);
    colorprint(add_timestamp(text_buff), ANSI_COLOR_YELLOW);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main(int argc, char *argv[])
{
    if (argc != 5)
        return err("Missing arguments passengers_num, trolleys_num, trolley_capacity, rides_num", -1);

    if (parse_int(argv[1], &passengers_num) < 0)
        return -1;
    if (parse_int(argv[2], &trolleys_num) < 0)
        return -1;
    if (parse_int(argv[3], &trolley_capacity) < 0)
        return -1;
    if (parse_int(argv[4], &rides_num) < 0)
        return -1;

    srand(time(NULL));
    start_millis = millis();
    riding_trolleys_num = trolleys_num;

    passengers = malloc(sizeof(passenger_t *) * passengers_num);

    trolleys = malloc(sizeof(trolley_t *) * trolleys_num);

    passengers_queue = malloc(sizeof(queue_t));
    quenew(passengers_queue, passengers_num);

    trolleys_queue = malloc(sizeof(queue_t));
    quenew(trolleys_queue, trolleys_num);

    for (int i = 0; i < passengers_num; i++)
    {
        passengers[i] = malloc(sizeof(passenger_t));
        passengers[i]->id = i;
        passengers[i]->state = STATE_RELEASED;
        queadd(passengers_queue, passengers[i]);

        if (pthread_create(&passengers[i]->thread, NULL, passenger_thread_fun, passengers[i]) < 0)
            return err("Cannot create passenger thread", 1);
    }

    for (int i = 0; i < trolleys_num; i++)
    {
        trolleys[i] = malloc(sizeof(trolley_t));
        trolleys[i]->id = i;
        trolleys[i]->passengers = malloc(sizeof(passenger_t *) * trolley_capacity);
        queadd(trolleys_queue, trolleys[i]);

        if (pthread_create(&trolleys[i]->thread, NULL, trolley_thread_fun, trolleys[i]) < 0)
            return err("Cannot create trolley thread", 1);
    }

    for (int i = 0; i < trolleys_num; i++)
        pthread_join(trolleys[i]->thread, NULL);

    for (int i = 0; i < passengers_num; i++)
        pthread_join(passengers[i]->thread, NULL);

    queclear(passengers_queue);
    queclear(trolleys_queue);
    free(passengers_queue);
    free(trolleys_queue);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_add_passenger_to_queue);
    pthread_cond_destroy(&cond_select_passenger_to_board);
    pthread_cond_destroy(&cond_passenger_board);
    pthread_cond_destroy(&cond_select_passenger_to_click);
    pthread_cond_destroy(&cond_passenger_onboard_action);
    pthread_cond_destroy(&cond_platform_queue_change);
    pthread_cond_destroy(&cond_select_passenger_to_release);
    pthread_cond_destroy(&cond_passenger_release);

    for (int i = 0; i < passengers_num; i++)
        free(passengers[i]);
    free(passengers);

    for (int i = 0; i < trolleys_num; i++)
    {
        free(trolleys[i]->passengers);
        free(trolleys[i]);
    }
    free(trolleys);

    return 0;
}