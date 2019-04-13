#include "../shared_files/config.h"
#include "../sysv_shared_files/config_spec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "../client_files/cparser.h"
#include "../client_files/client_helper.h"
#include "../shared_files/msgtype_converter.h"
#include "../sysv_shared_files/local_msgtypes.h"
#include "../shared_files/colors.h"
#include "../shared_files/global_msgtypes.h"
#include "../sysv_shared_files/structures.h"
#include <mqueue.h>
#include <fcntl.h>
#include <signal.h>
#include <mqueue.h>

char *buffer;
char *buffer2;
char *client_path;

mqd_t server_queue_id;
int client_id;
mqd_t client_queue_id;
int fork_pid;

int send_message(int global_type, char *message)
{
    char type = convert_to_local(global_type);
    snprintf(buffer, MAX_STATEMENT_SIZE, "%d%c%s", client_id, type, message);
    if (mq_send(server_queue_id, buffer, MAX_STATEMENT_SIZE, get_priority_by_type(global_type)) == -1)
        return perr("Send message error", -1);
    return 0;
}
void stop()
{
    send_message(TYPE_STOP, "");
    kill(fork_pid, SIGKILL);
    exit(0);
}
void stop_client()
{
    stop();
}

void exit_fun()
{
    mq_close(client_queue_id);
    mq_unlink(client_path);
}

void handleSIGINT(int signal)
{
    stop();
}

int main(int argc, char *argv[])
{

    printf("Server path: %s \n", get_server_path());

    if ((server_queue_id = mq_open(get_server_path(), O_WRONLY)) < 0)
        pdie("Cannot open server IPC", 1);

    client_path = malloc(sizeof(char) * 30);
    get_client_path(client_path);
    if ((client_queue_id = mq_open(client_path, O_WRONLY)) >= 0)
    {
        mq_close(client_queue_id);
        mq_unlink(get_server_path());
        printf("Deleting old queue\n");
    }

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_STATEMENT_SIZE;
    attr.mq_flags = 0;

    if ((client_queue_id = mq_open(client_path, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) < 0)
        pdie("Cannot open IPC", 1);

    atexit(exit_fun);

    buffer = malloc(sizeof(char) * MAX_STATEMENT_SIZE);
    buffer2 = malloc(sizeof(char) * MAX_STATEMENT_SIZE);

    snprintf(buffer, 255, "%d%c%s", -1, MTYPE_INIT, client_path);

    if (mq_send(server_queue_id, buffer, MAX_STATEMENT_SIZE, get_priority_by_type(TYPE_INIT)) == -1)
        pdie("Send message error", 2);

    if (mq_receive(client_queue_id, buffer, MAX_STATEMENT_SIZE, 0) == -1)
        pdie("Receive message error", 2);

    sscanf(buffer, "%d", &client_id);

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
            if (mq_receive(client_queue_id, buffer, MAX_STATEMENT_SIZE, 0) == -1)
            {

                kill(getppid(), 9);
                pdie("Receive message error", 2);
            }

            int client_id2;
            if (sscanf(buffer, "%d%[^\t\n]", &client_id2, buffer2) < 1)
                die("Error parsing message", 2);
            if (strlen(buffer2) >= 5 && strncmp(buffer2, "@STOP", 5) == 0)
                stop();
            print_server_response(buffer2);
        }
    }
}
