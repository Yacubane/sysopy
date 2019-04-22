#include "server_files/clients_handler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>

typedef struct client_t
{
    int id;
    mqd_t queue_id;
    int is_friend;
} client_t;

static char *buffer;
static int cilent_no;
static client_t **clients;

static int send_message_(client_t *client, char *message)
{
    snprintf(buffer, MAX_STATEMENT_SIZE, "%d%s", client->id, message);
    if (mq_send(client->queue_id, buffer, MAX_STATEMENT_SIZE, 1) < 0)
        return -1;
    return 0;
}

int init_clients_handler()
{
    clients = calloc(MAX_CLIENTS_SIZE, sizeof(client_t *));
    cilent_no = 1;
    buffer = malloc(sizeof(char) * MAX_STATEMENT_SIZE);
    return 0;
}
int init_client(char *message)
{

    int queue_id;
    if ((queue_id = mq_open(message, O_WRONLY)) < 0)
        return err("Cannot open client IPC", 2);

    printf("Opened client queue %d\n", queue_id);
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
    {
        if (clients[i] == NULL)
        {
            clients[i] = malloc(sizeof(client_t));
            clients[i]->id = cilent_no++;
            clients[i]->queue_id = queue_id;
            clients[i]->is_friend = 0;

            char num_buffer[10];
            snprintf(num_buffer, 10, "%d", clients[i]->id);

            strcpy(buffer, "");
            strcpy(buffer, num_buffer);

            if (mq_send(clients[i]->queue_id, buffer, MAX_STATEMENT_SIZE, 1) < 0)
                outperr("Cannot send message");

            return 0;
        }
    }

    return err("Cannot add more clients", 3);
}

int get_client_arr_id(int client_id)
{
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL && clients[i]->id == client_id)
            return i;
    return -1;
}

int get_client_array_size()
{
    return MAX_CLIENTS_SIZE;
}

int is_client(int array_id)
{
    if (clients[array_id] == NULL)
        return 0;
    return 1;
}

int is_client_friend(int array_id)
{
    if (clients[array_id]->is_friend == 1)
        return 1;
    return 0;
}

int set_client_friend(int array_id, int is_friend)
{
    clients[array_id]->is_friend = is_friend;
    return 0;
}

int get_client_id(int array_id)
{
    return clients[array_id]->id;
}

int send_message(int array_id, char *message)
{
    return send_message_(clients[array_id], message);
}

int free_client(int i)
{
    if (clients[i] != NULL)
    {
        free(clients[i]);
        clients[i] = NULL;
    }
    return 0;
}