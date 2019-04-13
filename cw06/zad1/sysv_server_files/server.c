#include <stdio.h>
#include "../shared/utils.h"
#include "../shared/shared.h"
#include "../shared/errors.h"
#include "../server_files/msghdl.h"
#include "../sysv_shared_files/types.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
int msgid;
msgbuf_t *buffer;
void handleSIGINT(int signal)
{
    printf("Shutting down server\n");
    int clients_num = shutdown();
    printf("Waiting for %d clients to stop\n", clients_num);

    while (clients_num > 0)
    {
        if (msgrcv(msgid, buffer, sizeof(msgbuf_t) - sizeof(long), -MTYPE_MAX, 0) == -1)
            pdie("Receive message error", 2);
        handle_message(buffer->mid, convert_type(buffer->mtype), buffer->mtext);
        int new_clients_num = get_clients_num();
        if (new_clients_num != clients_num)
        {
            printf("Shutdown client with id: %d\n", buffer->mid);
            clients_num = new_clients_num;
        }
    }

    exit(0);
}

void exit_fun() {
    struct msqid_ds ms;
    msgctl(msgid, IPC_RMID, &ms);
}

int main(int argc, char *argv[])
{
    init_msghdl();

    key_t key = get_server_key();
    if ((msgid = msgget(key, 0)) >= 0)
    {
        struct msqid_ds ms;
        msgctl(msgid, IPC_RMID, &ms);
        printf("Deleting old IPC\n");
    }

    if ((msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0)
        pdie("Cannot open IPC", 1);
        
    atexit(exit_fun);

    struct sigaction act;
    act.sa_handler = handleSIGINT;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    buffer = malloc(sizeof(msgbuf_t));
    while (1)
    {
        if (msgrcv(msgid, buffer, sizeof(msgbuf_t) - sizeof(long), -MTYPE_MAX, 0) == -1)
            pdie("Receive message error", 2);
        handle_message(buffer->mid, convert_type(buffer->mtype), buffer->mtext);
    }
    free(buffer);
}