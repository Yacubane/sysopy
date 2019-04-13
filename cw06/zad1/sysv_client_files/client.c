#include "../shared/shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "../client_files/cparser.h"
#include "../sysv_shared_files/types.h"
#include "../shared/colors.h"

msgbuf_t *msg_buffer;
msgbuf_t *cmsg_buffer;
int server_msgid;
int client_id;
int client_msgid;
int fork_pid;

int send_message(int type, char *message)
{
    msg_buffer->mtype = type;
    msg_buffer->mid = client_id;
    strcpy(msg_buffer->mtext, message);
    if (msgsnd(server_msgid, msg_buffer, sizeof(msgbuf_t) - sizeof(long), 0) < 0)
        return perr("Send message error", -1);
    return 0;
}
void stop()
{
    send_message(MTYPE_STOP, "");
    kill(fork_pid, SIGKILL);
    exit(0);
}

void exit_fun() {
    struct msqid_ds ms;
    msgctl(client_msgid, IPC_RMID, &ms);
}

key_t get_client_key(char proj_id)
{
    return ftok(get_home_dir(), proj_id);
}

void handleSIGINT(int signal)
{
    stop();
}

int main(int argc, char *argv[])
{

    key_t server_key = get_server_key();
    printf("Key: %d \n", server_key);

    if ((server_msgid = msgget(server_key, 0)) < 0)
        pdie("Cannot open server IPC", 1);

    key_t key = get_client_key(argv[1][0]);

    if ((client_msgid = msgget(key, 0)) >= 0)
    {
        struct msqid_ds ms;
        msgctl(client_msgid, IPC_RMID, &ms);
        printf("Deleting old IPC\n");
    }

    if ((client_msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0)
        pdie("Cannot open IPC", 1);

    atexit(exit_fun);


    msg_buffer = malloc(sizeof(msgbuf_t));
    cmsg_buffer = malloc(sizeof(cmsgbuf_t));

    char key_buffer[255];
    snprintf(key_buffer, 255, "%d", key);
    msg_buffer->mtype = MTYPE_INIT;
    msg_buffer->mid = 0;
    strcpy(msg_buffer->mtext, key_buffer);
    msgsnd(server_msgid, msg_buffer, sizeof(msgbuf_t) - sizeof(long), 0);

    if (msgrcv(client_msgid, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0, 0) == -1)
        pdie("Receive message error", 2);

    client_id = cmsg_buffer->mtype;
    printf("Client ID: %d \n", client_id);

    fork_pid = fork();
    if (fork_pid > 0)
    {
        struct sigaction act;
        act.sa_handler = handleSIGINT;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGINT, &act, NULL);
        run_cparser();
    }
    else if (fork_pid == 0)
    {
        while (1)
        {
            if (msgrcv(client_msgid, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0, 0) == -1)
            {
                kill(getppid(), 9);
                pdie("Receive message error", 2);
            }

            if (strlen(cmsg_buffer->mtext) >= 5 && strncmp(cmsg_buffer->mtext, "@STOP", 5) == 0)
                stop();

            if (strlen(cmsg_buffer->mtext) >= 5 && strncmp(cmsg_buffer->mtext, "ERROR", 5) == 0)
                set_color(ANSI_COLOR_RED);
            else if (strlen(cmsg_buffer->mtext) >= 7 && strncmp(cmsg_buffer->mtext, "SUCCESS", 7) == 0)
                set_color(ANSI_COLOR_GREEN);
            else if (strlen(cmsg_buffer->mtext) >= 7 && strncmp(cmsg_buffer->mtext, "WARNING", 7) == 0)
                set_color(ANSI_COLOR_YELLOW);
            else
                set_color(ANSI_COLOR_CYAN);
            printf("SERVER: %s", cmsg_buffer->mtext);
            reset_color();
        }
    }
}
void send_echo(char *message)
{
    send_message(MTYPE_ECHO, message);
}
void send_list()
{
    send_message(MTYPE_LIST, "");
}
void send_friends(int *friends, int friends_size, int array_size)
{
    char buffer[255];
    char num_buffer[10];
    strcpy(buffer, "");
    for (int i = 0; i < friends_size; i++)
    {
        snprintf(num_buffer, 10, "%d", friends[i]);
        if (i < (friends_size - 1))
            strcat(num_buffer, " ");
        strcat(buffer, num_buffer);
    }

    send_message(MTYPE_FRIENDS, buffer);
}
void send_add(int id)
{
    char buffer[255];
    snprintf(buffer, 255, "%d", id);
    send_message(MTYPE_ADD, buffer);
}
void send_del(int id)
{
    char buffer[255];
    snprintf(buffer, 255, "%d", id);
    send_message(MTYPE_DEL, buffer);
}
void send_to_all(char *message)
{
    send_message(MTYPE_2ALL, message);
}
void send_to_friends(char *message)
{
    send_message(MTYPE_2FRIENDS, message);
}
void send_to_one(int id, char *message)
{
    char buffer[255];
    snprintf(buffer, 255, "%d %s", id, message);
    send_message(MTYPE_2ONE, buffer);
}
void send_stop()
{
    stop();
}
