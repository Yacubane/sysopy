#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include "../server_files/msghdl_spec.h"
#include "../server_files/msghdl.h"
#include "../shared/errors.h"
#include "../shared/utils.h"
#include "../shared/shared.h"
#include "../sysv_shared_files/types.h"

typedef struct client_t {
    int id;
    key_t queue_id;
    int is_friend;
} client_t;

static cmsgbuf_t *cmsg_buffer;
static int cilent_no;
static client_t **clients;
int send_message(client_t *client, char *message)
{
    cmsg_buffer->mtype = client->id;
    strcpy(cmsg_buffer->mtext, message);
    if ((msgsnd(client->queue_id, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0)) < 0)
        return -1;
    return 0;
}
int send_success_message(client_t *client, char *message)
{
    char buffer[MAX_STATEMENT_SIZE];
    snprintf(buffer, MAX_STATEMENT_SIZE, "%s%s", "SUCCESS - ", message);
    if (send_message(client, buffer) < 0)
        outerr("Cannot send message");
    return 0;
}
int send_error_message(client_t *client, char *message)
{
    char buffer[MAX_STATEMENT_SIZE];
    snprintf(buffer, MAX_STATEMENT_SIZE, "%s%s", "ERROR - ", message);
    if (send_message(client, buffer) < 0)
        outerr("Cannot send message");
    return 0;
}
int send_warning_message(client_t *client, char *message)
{
    char buffer[MAX_STATEMENT_SIZE];
    snprintf(buffer, MAX_STATEMENT_SIZE, "%s%s", "WARNING - ", message);
    if (send_message(client, buffer) < 0)
        outerr("Cannot send message");
    return 0;
}

char *create_message(int client_id, const char *message)
{
    char *final_message = malloc(sizeof(char) * 255);
    char date_buffer[50];
    current_time(date_buffer, 50);

    snprintf(final_message, 255, "%s [%d] [%s] \"%s\"", "Message from client with ID:", client_id, date_buffer, message);
    return final_message;
}
int init_msghdl_spec()
{
    clients = calloc(MAX_CLIENTS_SIZE, sizeof(client_t *));
    cilent_no = 1;
    cmsg_buffer = malloc(sizeof(cmsgbuf_t));
    return 0;
}
client_t *find_client_by_id(int client_id)
{
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL && clients[i]->id == client_id)
            return clients[i];
    return NULL;
}

int handle_init(char *message)
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

int handle_echo(int client_id, char *message)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);

    if (send_message(client, message) < 0)
        outerr("Cannot send message");

    return 0;
}
int handle_list(int client_id)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);

    char buffer[MAX_STATEMENT_SIZE];
    printf("Printing list of clients:\n");
    send_success_message(client, "Printing list of clients");
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
    {
        if (clients[i] != NULL)
        {
            snprintf(buffer, MAX_STATEMENT_SIZE, "Client[%d] with id: %d", i, clients[i]->id);
            printf("Client[%d] with id: %d\n", i, clients[i]->id);
            send_message(client, buffer);
        }
    }
    return 0;
}
int handle_friends(int client_id, int *clients_id, int friends_size, int array_size)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);

    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL) clients[i]->is_friend = 0;

    for (int i = 0; i < friends_size; i++)
        handle_add(client_id, clients_id[i]);

    if(friends_size == 0)
        send_success_message(client, "Cleared all friends");

    return 0;
}
int handle_add(int client_id, int to_add_client_id)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);

    client_t *to_add_client;
    if ((to_add_client = find_client_by_id(to_add_client_id)) == NULL)
    {
        send_error_message(client, "Adding client - client doesn't exist");
        return err("Cannot find client by id", -1);
    }
    if (to_add_client->is_friend == 1)
    {
        send_warning_message(client, "This client is friend already");
        return 1;
    }
    to_add_client->is_friend = 1;
    send_success_message(client, "Added new friend");
    return 0;
}
int handle_del(int client_id, int to_del_client_id)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);

    client_t *to_del_client;
    if ((to_del_client = find_client_by_id(to_del_client_id)) == NULL)
    {
        send_error_message(client, "Deleting client - client doesn't exist");
        return err("Cannot find client by id", -1);
    }
    if (to_del_client->is_friend == 0)
    {
        send_warning_message(client, "This client isn't friend already");
        return 1;
    }
    to_del_client->is_friend = 0;
    send_success_message(client, "Deleted friend");
    return 0;
}

int handle_to_all(int client_id, char *message)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);
    char *final_message = create_message(client_id, message);
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL && clients[i]->id != client_id)
            if (send_message(clients[i], final_message) < 0)
                outerr("Cannot send message");
    free(final_message);
    send_success_message(client, "Sent message to all");
    return 0;
}
int handle_to_friends(int client_id, char *message)
{
    client_t *client;
    if ((client = find_client_by_id(client_id)) == NULL)
        return err("Cannot find client by id", -1);
    char *final_message = create_message(client_id, message);
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL && clients[i]->is_friend == 1)
            if (send_message(clients[i], final_message) < 0)
                outerr("Cannot send message");
    free(final_message);
    send_success_message(client, "Sent message to friends");
    return 0;
}
int handle_to_one(int from_client_id, int to_client_id, char *message)
{
    client_t *client;
    if ((client = find_client_by_id(from_client_id)) == NULL)
        return err("Cannot find client by id", -1);

    char *final_message = create_message(from_client_id, message);
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL && clients[i]->id == to_client_id)
        {
            if (send_message(clients[i], final_message) < 0)
                outerr("Cannot send message");
            free(final_message);
            send_success_message(client, "Sent message to one");

            return 0;
        }

    free(final_message);
    outerr("Cannot find receiver 2ONE");
    send_error_message(client, "Couldn't find receiver");
    return -1;
}
int handle_stop(int client_id)
{
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL && clients[i]->id == client_id)
        {
            free(clients[i]);
            clients[i] = NULL;
            return 0;
        }
    outerr("Couldn't find client to STOP");
    return -1;
}

int shutdown()
{
    int sent_to = 0;
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL)
        {
            if (send_message(clients[i], "@STOP") == 0)
            {
                printf("Sent shutdown signal to client %d\n", clients[i]->id);
                sent_to++;
            }
        }
    return sent_to;
}

int get_clients_num()
{
    int clients_num = 0;
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
        if (clients[i] != NULL)
            clients_num++;
    return clients_num;
}
int convert_type(int type)
{
    switch (type)
    {
    case MTYPE_STOP:
        return TYPE_STOP;
    case MTYPE_LIST:
        return TYPE_LIST;
    case MTYPE_FRIENDS:
        return TYPE_FRIENDS;
    case MTYPE_ADD:
        return TYPE_ADD;
    case MTYPE_DEL:
        return TYPE_DEL;
    case MTYPE_ECHO:
        return TYPE_ECHO;
    case MTYPE_2ALL:
        return TYPE_2ALL;
    case MTYPE_2FRIENDS:
        return TYPE_2FRIENDS;
    case MTYPE_2ONE:
        return TYPE_2ONE;
    case MTYPE_INIT:
        return TYPE_INIT;
    }
    return -1;
}
