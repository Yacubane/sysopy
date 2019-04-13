#include <stdio.h>
#include "../shared_files/utils.h"
#include "../shared_files/config.h"
#include "../sysv_shared_files/config_spec.h"
#include "../shared_files/errors.h"
#include "../server_files/msghdl.h"
#include "../shared_files/msgtype_converter.h"
#include "../sysv_shared_files/local_msgtypes.h"
#include "../sysv_shared_files/structures.h"
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <mqueue.h>
mqd_t queue_id;
char *buffer;
char *buffer2;
int receive_message()
{
    if (mq_receive(queue_id, buffer, MAX_STATEMENT_SIZE, 0) == -1)
        pdie("Receive message error", 2);

    int client_id;
    char type;

    if (sscanf(buffer, "%d%c%[^\t\n]", &client_id, &type, buffer2) < 2)
        die("Error parsing message", 2);
    
    handle_message(client_id, convert_to_global(type), buffer2);
    return client_id;
}
void handleSIGINT(int signal)
{
    printf("Shutting down server\n");
    int clients_num = shutdown();
    printf("Waiting for %d clients to stop\n", clients_num);

    while (clients_num > 0)
    {
        int client_id = receive_message();

        int new_clients_num = get_clients_num();
        if (new_clients_num != clients_num)
        {
            printf("Shutdown client with id: %d\n", client_id);
            clients_num = new_clients_num;
        }
    }

    exit(0);
}

void exit_fun()
{
    mq_close(queue_id);
    mq_unlink(get_server_path());
    free(buffer);
    free(buffer2);
}

int main(int argc, char *argv[])
{
    init_msghdl();

    queue_id = 0;
    if ((queue_id = mq_open(get_server_path(), O_WRONLY)) >= 0)
    {
        mq_close(queue_id);
        mq_unlink(get_server_path());
        printf("Deleting old queue\n");
    }

    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_STATEMENT_SIZE;
    attr.mq_flags = 0;

    if ((queue_id = mq_open(get_server_path(), O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) < 0)
        pdie("Cannot open queue", 1);

    buffer = malloc(sizeof(char) * MAX_STATEMENT_SIZE);
    buffer2 = malloc(sizeof(char) * MAX_STATEMENT_SIZE);

    atexit(exit_fun);
    struct sigaction act;
    act.sa_handler = handleSIGINT;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    while (1)
    {
        receive_message();
    }
    free(buffer);
    free(buffer2);
}