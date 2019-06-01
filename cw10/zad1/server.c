#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "errors.h"
#include "utils.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define PORT 5555
#define MAXMSG 512

#define MODE_TCP 1
#define MODE_UDP 0

#define CLIENT_ID_SIZE 10
#define CLIENTS_SIZE 1000

#define CLIENT_STATE_NULL 0
#define CLIENT_STATE_BUSY 1
#define CLIENT_STATE_IDLE 2

int request_counter = 0;

void timeval_diff(struct timeval* start,
                  struct timeval* stop,
                  struct timeval* diff) {
  if ((stop->tv_usec - start->tv_usec) < 0) {
    diff->tv_sec = stop->tv_sec - start->tv_sec - 1;
    diff->tv_usec = 1000000 + stop->tv_usec - start->tv_usec;
  } else {
    diff->tv_sec = stop->tv_sec - start->tv_sec;
    diff->tv_usec = stop->tv_usec - start->tv_usec;
  }
}

int sock1;
int sock2;
fd_set active_fd_set, read_fd_set;

typedef struct client_t {
  char id[CLIENT_ID_SIZE];
  int state;
  struct timeval last_ping_time;
  int sockid;
} client_t;

client_t** clients;

int mode = MODE_TCP;
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

client_t* get_client_with_socket_id(int sockid) {
  client_t* client = NULL;
  for (int j = 0; j < CLIENTS_SIZE; j++)
    if (clients[j]->sockid == sockid) {
      client = clients[j];
      break;
    }
  return client;
}
#define MSGTYPE_PING 1
#define MSGTYPE_RESULT 2
#define MSGTYPE_REQUEST 3
int read_from_client(client_t* client) {
  char msgtype;
  int nbytes;

  nbytes = read(client->sockid, &msgtype, 1);
  if (nbytes <= 0) {
    return -1;
  } else {
    if (msgtype == MSGTYPE_PING) {
      gettimeofday(&client->last_ping_time, NULL);
    } else if (msgtype == MSGTYPE_RESULT) {
      printf("Result\n");

      int response_id = 0;
      recv(client->sockid, &response_id, sizeof(int), MSG_WAITALL);
      int words_len = 0;
      recv(client->sockid, &words_len, sizeof(int), MSG_WAITALL);
      int response_size = 0;
      recv(client->sockid, &response_size, sizeof(int), MSG_WAITALL);
      char* response = malloc(sizeof(char) * response_size);
      char* response_pointer = response;
      int remainging_bytes = response_size;

      while (remainging_bytes > 0) {
        int nbytes = recv(client->sockid, response_pointer,
                          MIN(remainging_bytes, 1024), MSG_WAITALL);
        response_pointer += nbytes;
        remainging_bytes -= nbytes;
      }
      printf("Response words: %s", response);
      printf("Response [%d]: word count: %d\n", response_id, words_len);

      free(response);
    }
    return 0;
  }
}

int make_inet_socket(uint16_t port) {
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  if (mode == MODE_TCP) {
    sock = socket(PF_INET, SOCK_STREAM, 0);

  } else {
    sock = socket(PF_INET, SOCK_DGRAM, 0);
  }
  int true = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
    perror("bind1");
    exit(EXIT_FAILURE);
  }

  return sock;
}

int make_unix_socket(char* path) {
  int sock;
  struct sockaddr_un name;

  if (mode == MODE_TCP) {
    sock = socket(PF_UNIX, SOCK_STREAM, 0);

  } else {
    sock = socket(PF_UNIX, SOCK_DGRAM, 0);
  }
  int true = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sun_family = AF_UNIX;
  strcpy(name.sun_path, path);
  unlink(path);
  if (bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
    perror("bind2");
    exit(EXIT_FAILURE);
  }

  return sock;
}

void* ping_thread_fun(void* data) {
  while (1) {
    sleep(2);
    struct timeval actual_time;
    gettimeofday(&actual_time, NULL);
    struct timeval diff;
    for (int j = 0; j < CLIENTS_SIZE; j++)
      if (clients[j]->state != CLIENT_STATE_NULL) {
        timeval_diff(&clients[j]->last_ping_time, &actual_time, &diff);
        if (diff.tv_sec > 5) {
          close(clients[j]->sockid);
          FD_CLR(clients[j]->sockid, &active_fd_set);
          clients[j]->state = CLIENT_STATE_NULL;
        }
      }

    pthread_mutex_lock(&socket_mutex);
    for (int i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, &active_fd_set)) {
        if (i == sock1 || i == sock2) {
        } else {
          char msgsend = MSGTYPE_PING;
          send(i, &msgsend, 1, 0);
        }
      }
    }
    pthread_mutex_unlock(&socket_mutex);
  }

  return NULL;
}

int send_request(char* path) {
  printf("%s a\n", path);
  FILE* f = fopen(path, "r");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  char* string = malloc(fsize + 1);
  fread(string, 1, fsize, f);
  fclose(f);
  string[fsize] = 0;

  int blocks_num = 0;
  int BUFFER_SIZE = 1024;
  int pointer = 0;
  while (pointer != fsize) {
    int old_pointer = pointer;
    int end_pointer = old_pointer + BUFFER_SIZE;
    if (end_pointer >= fsize) {
      end_pointer = fsize;
    } else {
      while (string[end_pointer] != ' ')
        end_pointer--;
    }
    blocks_num++;
    pointer = end_pointer;
  }

  pthread_mutex_lock(&clients_mutex);
  client_t* client = NULL;
  for (int j = 0; j < CLIENTS_SIZE; j++) {
    if (clients[j]->state == CLIENT_STATE_IDLE) {
      client = clients[j];
      break;
    }
  }

  if (client == NULL) {
    for (int j = 0; j < CLIENTS_SIZE; j++) {
      if (clients[j]->state != CLIENT_STATE_NULL) {
        client = clients[j];
        break;
      }
    }
  }
  if (client != NULL) {
    client->state = CLIENT_STATE_BUSY;
  }
  pthread_mutex_unlock(&clients_mutex);

  if (client == NULL) {
    return -1;
  }

  pthread_mutex_lock(&socket_mutex);

  char type = MSGTYPE_REQUEST;
  request_counter++;
  printf("elo\n");
  send(client->sockid, &type, 1, 0);
  send(client->sockid, &request_counter, sizeof(int), 0);
  send(client->sockid, &fsize, sizeof(int), 0);
  send(client->sockid, string, fsize, 0);

  pthread_mutex_unlock(&socket_mutex);

  free(string);
}

int main(int argc, char* argv[]) {
  if (argc != 3)
    return err("Missing arguments <port> <unix_path>", -1);

  int port = 0;
  if (parse_int(argv[1], &port) < 0)
    return err("Wrong port", -1);

  clients = malloc(sizeof(client_t*) * CLIENTS_SIZE);
  for (int i = 0; i < CLIENTS_SIZE; i++) {
    clients[i] = malloc(sizeof(client_t) * CLIENTS_SIZE);
    clients[i]->state = CLIENT_STATE_NULL;
  }

  int i;
  struct sockaddr_in clientname;
  socklen_t size;

  /* Create the socket and set it up to accept connections. */
  sock1 = make_inet_socket(port);
  sock2 = make_unix_socket(argv[2]);

  if (mode == MODE_TCP)
    if (listen(sock1, 1) < 0 || listen(sock2, 1) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }

  /* Initialize the set of active sockets. */
  FD_ZERO(&active_fd_set);
  FD_SET(STDIN_FILENO, &active_fd_set);
  FD_SET(sock1, &active_fd_set);
  FD_SET(sock2, &active_fd_set);

  pthread_t id;
  int data = 0;
  int res = pthread_create(&id, NULL, ping_thread_fun, &data);
  printf("%d\n", res);
  while (1) {
    /* Block until input arrives on one or more active sockets. */
    read_fd_set = active_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    /* Service all the sockets with input pending. */
    for (i = 0; i < FD_SETSIZE; ++i)
      if (FD_ISSET(i, &read_fd_set)) {
        if (i == STDIN_FILENO) {
          char path[128];
          size_t sz = read(0, path, sizeof(path));
          path[sz - 1] = '\0';
          send_request(path);
        } else if (i == sock1 || i == sock2) {
          pthread_mutex_lock(&socket_mutex);

          if (mode == MODE_TCP) {
            /* Connection request on original socket. */
            size_t new;
            size = sizeof(clientname);
            new = accept(i, (struct sockaddr*)&clientname, &size);
            if (new < 0) {
              perror("accept");
              exit(EXIT_FAILURE);
            }

            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            setsockopt(new, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,
                       sizeof tv);

            if (i == sock1)
              fprintf(stderr, "Server: connect from host %s, port %hd.\n",
                      inet_ntoa(clientname.sin_addr),
                      ntohs(clientname.sin_port));
            FD_SET(new, &active_fd_set);

            int found_index = -1;
            for (int j = 0; j < CLIENTS_SIZE; j++)
              if (clients[j]->state == CLIENT_STATE_NULL) {
                gettimeofday(&clients[j]->last_ping_time, NULL);
                clients[j]->sockid = new;
                found_index = j;
                break;
              }
            if (found_index == -1) {
              printf("Exceeded clients size\n");
              exit(2137);
            }

            int nbytes = recv(new, clients[found_index]->id, CLIENT_ID_SIZE, 0);
            if (nbytes > 0) {
              printf("Added new client\n");
              clients[found_index]->state = CLIENT_STATE_IDLE;
            } else {
              printf("Error adding new client\n");
            }

            // char type = MSGTYPE_REQUEST;
            // send(new, &type, 1, 0);
            // int num = 0;
            // send(new, &num, sizeof(int), 0);
            // num = 2;
            // send(new, &num, sizeof(int), 0);
            // num = 6;
            // send(new, &num, sizeof(int), 0);
            // send(new, "BLOCK1", 6, 0);
            // send(new, &num, sizeof(int), 0);
            // send(new, "BLOCK2", 6, 0);

            if (i == sock2)
              printf("Added new unix socket\n");
          } else {
            // read_from_client(i);
          }
          pthread_mutex_unlock(&socket_mutex);

        } else {
          /* Data arriving on an already-connected socket. */
          client_t* client = get_client_with_socket_id(i);

          if (read_from_client(client) < 0) {
            close(i);
            printf("Client [%s] disconnected\n", client->id);
            FD_CLR(i, &active_fd_set);
            client->state = CLIENT_STATE_NULL;
          }
        }
      }
  }
}
