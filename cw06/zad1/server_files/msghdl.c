#include "msghdl.h"

char *buffer;
int init_msghdl()
{
    init_msghdl_spec();
    buffer = malloc(sizeof(char) * MAX_STATEMENT_SIZE);
    return 0;
}

void handle_message(int client_id, int type, char *message)
{
    switch (type)
    {
    case TYPE_STOP:
    {
        handle_stop(client_id);
        break;
    }
    case TYPE_LIST:
    {
        handle_list(client_id);
        break;
    }
    case TYPE_FRIENDS:
    {
        int friends_no = 0;
        for (int i = 0; i < strlen(message); i++)
        {
            if (message[i] == ' ')
                friends_no++;
        }

        int friends[friends_no];

        char *ptr = strtok(message, " \n");
        int i = 0;
        while (ptr != NULL)
        {
            int friend_id;
            if (parse_num(ptr, &friend_id) < 0)
                outerr("Cannot parse friend id");
            else
                friends[i++] = friend_id;
            ptr = strtok(NULL, " \n");
        }
        handle_friends(client_id, friends, i, friends_no);
        break;
    }
    case TYPE_ADD:
    {
        int to_client_id;
        if (sscanf(message, "%d", &to_client_id) <= 0)
            outerr("Error while parsing ADD");
        handle_add(client_id, to_client_id);
        break;
    }
    case TYPE_DEL:
    {
        int to_client_id;
        if (sscanf(message, "%d", &to_client_id) <= 0)
            outerr("Error while parsing DEL");
        handle_del(client_id, to_client_id);
        break;
    }
    case TYPE_ECHO:
    {
        handle_echo(client_id, message);
        break;
    }
    case TYPE_2ALL:
    {
        handle_to_all(client_id, message);
        break;
    }
    case TYPE_2FRIENDS:
    {
        handle_to_friends(client_id, message);
        break;
    }
    case TYPE_2ONE:
    {
        int to_client_id;
        char buffer[MAX_STATEMENT_SIZE];
        if (sscanf(message, "%d %s", &to_client_id, buffer) <= 0)
            outerr("Error while parsing 2ONE");
        handle_to_one(client_id, to_client_id, buffer);
        break;
    }
    case TYPE_INIT:
    {
        handle_init(message);
        break;
    }
    default:
        outerr("Unknown message type");
        break;
    }
}