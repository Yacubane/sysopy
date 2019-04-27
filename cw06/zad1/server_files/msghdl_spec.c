#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include "msghdl_spec.h"
#include "../shared_files/errors.h"
#include "../shared_files/utils.h"
#include "../shared_files/config.h"
#include "clients_handler.h"

int send_success_message(int client_array_id, char *message)
{
    char buffer[MAX_STATEMENT_SIZE];
    snprintf(buffer, MAX_STATEMENT_SIZE, "%s%s", "SUCCESS - ", message);
    if (send_message(client_array_id, buffer) < 0)
        outerr("Cannot send message");
    return 0;
}
int send_error_message(int client_array_id, char *message)
{
    char buffer[MAX_STATEMENT_SIZE];
    snprintf(buffer, MAX_STATEMENT_SIZE, "%s%s", "ERROR - ", message);
    if (send_message(client_array_id, buffer) < 0)
        outerr("Cannot send message");
    return 0;
}
int send_warning_message(int client_array_id, char *message)
{
    char buffer[MAX_STATEMENT_SIZE];
    snprintf(buffer, MAX_STATEMENT_SIZE, "%s%s", "WARNING - ", message);
    if (send_message(client_array_id, buffer) < 0)
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
    return init_clients_handler();
}

int handle_init(char *message)
{
    return init_client(message);
}

int handle_echo(int client_id, char *message)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);

    if (send_message(client_arr_id, message) < 0)
        outerr("Cannot send message");

    return 0;
}
int handle_list(int client_id)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);

    char buffer[MAX_STATEMENT_SIZE];
    printf("Printing list of clients:\n");
    send_success_message(client_arr_id, "Printing list of clients");
    for (int i = 0; i < get_client_array_size(); i++)
    {
        if (is_client(i))
        {
            snprintf(buffer, MAX_STATEMENT_SIZE, "Client[%d] with id: %d", i, get_client_id(i));
            printf("Client[%d] with id: %d\n", i, get_client_id(i));
            send_message(client_arr_id, buffer);
        }
    }
    return 0;
}
int handle_friends(int client_id, int *clients_id, int friends_size, int array_size)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);

    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i))
            set_client_friend(client_arr_id, i, 0);

    for (int i = 0; i < friends_size; i++)
        handle_add(client_id, clients_id[i]);

    if (friends_size == 0)
        send_success_message(client_arr_id, "Cleared all friends");

    return 0;
}
int handle_add(int client_id, int to_add_client_id)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);

    int to_add_client_arr_id;
    if ((to_add_client_arr_id = get_client_arr_id(to_add_client_id)) == -1)
    {
        send_error_message(client_arr_id, "Adding client - client doesn't exist");
        return err("Cannot find client by id", -1);
    }
    if (is_client_friend(client_arr_id, to_add_client_arr_id))
    {
        send_warning_message(client_arr_id, "This client is friend already");
        return 1;
    }

    set_client_friend(client_arr_id, to_add_client_arr_id, 1);
    send_success_message(client_arr_id, "Added new friend");

    return 0;
}
int handle_del(int client_id, int to_del_client_id)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);

    int to_del_client_arr_id;
    if ((to_del_client_arr_id = get_client_arr_id(to_del_client_id)) == -1)
    {
        send_error_message(client_arr_id, "Deleting client - client doesn't exist");
        return err("Cannot find client by id", -1);
    }
    if (!is_client_friend(client_arr_id, to_del_client_arr_id))
    {
        send_warning_message(client_arr_id, "This client isn't friend already");
        return 1;
    }
    set_client_friend(client_arr_id, to_del_client_arr_id, 0);
    send_success_message(client_arr_id, "Deleted friend");
    return 0;
}

int handle_to_all(int client_id, char *message)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);
    char *final_message = create_message(client_id, message);
    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i) && get_client_id(i) != client_id)
            if (send_message(i, final_message) < 0)
                outerr("Cannot send message");
    free(final_message);
    send_success_message(client_arr_id, "Sent message to all");
    return 0;
}
int handle_to_friends(int client_id, char *message)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(client_id)) == -1)
        return err("Cannot find client by id", -1);
    char *final_message = create_message(client_id, message);
    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i) && is_client_friend(client_arr_id, i))
            if (send_message(i, final_message) < 0)
                outerr("Cannot send message");
    free(final_message);
    send_success_message(client_arr_id, "Sent message to friends");
    return 0;
}
int handle_to_one(int from_client_id, int to_client_id, char *message)
{
    int client_arr_id;
    if ((client_arr_id = get_client_arr_id(from_client_id)) == -1)
        return err("Cannot find client by id", -1);

    char *final_message = create_message(from_client_id, message);
    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i) && get_client_id(i) == to_client_id)
        {
            if (send_message(i, final_message) < 0)
                outerr("Cannot send message");
            free(final_message);
            send_success_message(client_arr_id, "Sent message to one");

            return 0;
        }

    free(final_message);
    outerr("Cannot find receiver 2ONE");
    send_error_message(client_arr_id, "Couldn't find receiver");
    return -1;
}
int handle_stop(int client_id)
{
    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i) && get_client_id(i) == client_id)
        {
            free_client(i);
            return 0;
        }
    outerr("Couldn't find client to STOP");
    return -1;
}

int shutdown()
{
    int sent_to = 0;
    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i))
        {
            if (send_message(i, "@STOP") == 0)
            {
                printf("Sent shutdown signal to client %d\n", get_client_id(i));
                sent_to++;
            }
        }
    return sent_to;
}

int get_clients_num()
{
    int clients_num = 0;
    for (int i = 0; i < get_client_array_size(); i++)
        if (is_client(i))
            clients_num++;
    return clients_num;
}
