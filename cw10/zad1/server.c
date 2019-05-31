#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define PORT 5555
#define MAXMSG 512

int tcp = 0;

int read_from_client(int filedes) {
  char buffer[MAXMSG];
  int nbytes;

  nbytes = read(filedes, buffer, MAXMSG);
  if (nbytes < 0) {
    /* Read error. */
    perror("read");
    exit(EXIT_FAILURE);
  } else if (nbytes == 0)
    /* End-of-file. */
    return -1;
  else {
    /* Data read. */
    fprintf(stderr, "Server: got message: `%s'\n", buffer);
    return 0;
  }
}

int make_inet_socket(uint16_t port) {
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  if (tcp) {
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

int make_unix_socket(uint16_t port) {
  int sock;
  struct sockaddr_un name;

  /* Create the socket. */
  /* Create the socket. */
  if (tcp) {
    sock = socket(PF_UNIX, SOCK_STREAM, 0);

  } else {
    sock = socket(PF_UNIX, SOCK_DGRAM, 0);
  }
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sun_family = AF_UNIX;
  strcpy(name.sun_path, "/tmp/socket");
  unlink("/tmp/socket");
  if (bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
    perror("bind2");
    exit(EXIT_FAILURE);
  }

  return sock;
}

int main(int argc, char* argv[]) {
  int sock1;
  int sock2;

  fd_set active_fd_set, read_fd_set;
  int i;
  struct sockaddr_in clientname;
  socklen_t size;

  /* Create the socket and set it up to accept connections. */
  sock1 = make_inet_socket(PORT);
  sock2 = make_unix_socket(PORT);

  if (tcp)
    if (listen(sock1, 1) < 0 || listen(sock2, 1) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }

  /* Initialize the set of active sockets. */
  FD_ZERO(&active_fd_set);
  FD_SET(sock1, &active_fd_set);
  FD_SET(sock2, &active_fd_set);

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
        if (i == sock1 || i == sock2) {
          if (tcp) {
            /* Connection request on original socket. */
            size_t new;
            size = sizeof(clientname);
            new = accept(i, (struct sockaddr*)&clientname, &size);
            if (new < 0) {
              perror("accept");
              exit(EXIT_FAILURE);
            }
            if (i == sock1)
              fprintf(stderr, "Server: connect from host %s, port %hd.\n",
                      inet_ntoa(clientname.sin_addr),
                      ntohs(clientname.sin_port));
            FD_SET(new, &active_fd_set);
            if (i == sock2)
              printf("Added new unix socket\n");
          } else {
            read_from_client(i);
          }

        } else if (i == sock2) {
          read_from_client(i);
        } else {
          /* Data arriving on an already-connected socket. */
          printf("New data\n");
          if (read_from_client(i) < 0) {
            close(i);
            FD_CLR(i, &active_fd_set);
          }
        }
      }
  }
}
