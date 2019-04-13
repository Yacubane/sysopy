#include "../server_files/clients_handler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "../sysv_shared_files/structures.h"

typedef struct client_t
{
    int id;
    key_t queue_id;
    int is_friend;
} client_t;

static cmsgbuf_t *cmsg_buffer;
static int cilent_no;
static client_t **clients;

static int send_message_(client_t *client, char *message)
{
    cmsg_buffer->mtype = client->id;
    strcpy(cmsg_buffer->mtext, message);
    if ((msgsnd(client->queue_id, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0)) < 0)
        return -1;
    return 0;
}

int init_clients_handler()
{
    clients = calloc(MAX_CLIENTS_SIZE, sizeof(client_t *));
    cilent_no = 1;
    cmsg_buffer = malloc(sizeof(cmsgbuf_t));
    return 0;
}
int init_client(char *message)
{
    key_t key;
    if (sscanf(message, "%d", &key) <= 0)
        return err("Error while parsing INIT", 1);

    int msgid;
    if ((msgid = msgget(key, 0)) < 0)
        return err("Cannot open client IPC", 2);

    printf("Opened client queue %d\n", msgid);
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
    {
        if (clients[i] == NULL)
        {
            clients[i] = malloc(sizeof(client_t));
            clients[i]->id = cilent_no++;
            clients[i]->queue_id = msgid;
            clients[i]->is_friend = 0;

            cmsg_buffer->mtype = clients[i]->id;
            if ((msgsnd(msgid, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0)) < 0)
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