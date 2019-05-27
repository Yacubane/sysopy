#include "../shared_files/config.h"
#include "../sysv_shared_files/config_spec.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../client_files/client_helper.h"
#include "../client_files/cparser.h"
#include "../shared_files/colors.h"
#include "../shared_files/global_msgtypes.h"
#include "../shared_files/msgtype_converter.h"
#include "../sysv_shared_files/local_msgtypes.h"
#include "../sysv_shared_files/structures.h"

msgbuf_t* msg_buffer;
msgbuf_t* cmsg_buffer;
int server_msgid;
int client_id;
int client_msgid;
int fork_pid;

int send_message(int global_type, char* message) {
  msg_buffer->mtype = convert_to_local(global_type);
  msg_buffer->mid = client_id;
  strcpy(msg_buffer->mtext, message);
  if (msgsnd(server_msgid, msg_buffer, sizeof(msgbuf_t) - sizeof(long), 0) < 0)
    return perr("Send message error", -1);
  return 0;
}
void stop() {
  send_message(TYPE_STOP, "");
  kill(fork_pid, SIGKILL);
  exit(0);
}
void stop_client() {
  stop();
}

void exit_fun() {
  struct msqid_ds ms;
  msgctl(client_msgid, IPC_RMID, &ms);
}

key_t get_client_key(char proj_id) {
  return ftok(get_home_dir(), proj_id);
}

void handleSIGINT(int signal) {
  stop();
}

int main(int argc, char* argv[]) {
  key_t server_key = get_server_key();
  printf("Key: %d \n", server_key);

  if ((server_msgid = msgget(server_key, 0)) < 0)
    pdie("Cannot open server IPC", 1);

  key_t key = get_client_key(argv[1][0]);

  if ((client_msgid = msgget(key, 0)) >= 0) {
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

  if (msgrcv(client_msgid, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0,
             0) == -1)
    pdie("Receive message error", 2);

  client_id = cmsg_buffer->mtype;
  printf("Client ID: %d \n", client_id);

  fork_pid = fork();
  if (fork_pid > 0) {
    struct sigaction act;
    act.sa_handler = handleSIGINT;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    run_cparser();
  } else if (fork_pid == 0) {
    while (1) {
      if (msgrcv(client_msgid, cmsg_buffer, sizeof(cmsgbuf_t) - sizeof(long), 0,
                 0) == -1) {
        kill(getppid(), 9);
        pdie("Receive message error", 2);
      }
      if (strlen(cmsg_buffer->mtext) >= 5 &&
          strncmp(cmsg_buffer->mtext, "@STOP", 5) == 0)
        stop();
      print_server_response(cmsg_buffer->mtext);
    }
  }
}
