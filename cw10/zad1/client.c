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

void unix_tcp_test() {
  int sock1;
  struct sockaddr_un name;

  /* Create the socket. */
  sock1 = socket(PF_UNIX, SOCK_STREAM, 0);
  if (sock1 < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sun_family = AF_UNIX;
  strcpy(name.sun_path, "/tmp/socket");

  if (connect(sock1, (struct sockaddr*)&name, sizeof(name))) {
    perror("Connection failed");
  }

  char buffer[512];
  char* hello = "Hello from client";

  int a = sendto(sock1, hello, strlen(hello), 0, NULL, 0);

  printf("Hello message sent.%d %d\n", a, errno);
  perror("Aha");

  int n;
  socklen_t len;
  n = recvfrom(sock1, (char*)buffer, 512, MSG_WAITALL, (struct sockaddr*)&name,
               &len);
  buffer[n] = '\0';
  printf("Server : %s\n", buffer);

  close(sock1);
}


void unix_udp_test() {
  int sock1;
  struct sockaddr_un name;

  /* Create the socket. */
  sock1 = socket(PF_UNIX, SOCK_DGRAM, 0);
  if (sock1 < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sun_family = AF_UNIX;
  strcpy(name.sun_path, "/tmp/socket");


  char buffer[512];
  char* hello = "Hello from client";

  sendto(sock1, hello, strlen(hello), 0, (struct sockaddr*)&name, sizeof(name));

  printf("Hello message sent.\n");

  int n;
  socklen_t len;
  n = recvfrom(sock1, (char*)buffer, 512, MSG_WAITALL, (struct sockaddr*)&name,
               &len);
  buffer[n] = '\0';
  printf("Server : %s\n", buffer);

  close(sock1);
}

void inet_tcp_test() {
  int sock1;

  struct sockaddr_in name;

  /* Create the socket. */
  sock1 = socket(PF_INET, SOCK_STREAM, 0);
  if (sock1 < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons(PORT);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  if (connect(sock1, (struct sockaddr*)&name, sizeof(name))) {
    perror("Connection failed");
  }

  char buffer[512];
  char* hello = "Hello from client";

  sendto(sock1, hello, strlen(hello), 0, (struct sockaddr*)&name, sizeof(name));

  printf("Hello message sent.\n");
  int n;
  socklen_t len;
  n = recvfrom(sock1, (char*)buffer, 512, MSG_WAITALL, (struct sockaddr*)&name,
               &len);
  buffer[n] = '\0';
  printf("Server : %s\n", buffer);

  close(sock1);
}

void inet_udp_test() {
  int sock1;

  struct sockaddr_in name;

  /* Create the socket. */
  sock1 = socket(PF_INET, SOCK_DGRAM, 0);
  if (sock1 < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons(PORT);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  if (connect(sock1, (struct sockaddr*)&name, sizeof(name))) {
    perror("Connection failed");
  }

  char buffer[512];
  char* hello = "Hello from client";

  sendto(sock1, hello, strlen(hello), 0, (struct sockaddr*)&name, sizeof(name));

  printf("Hello message sent.\n");
  int n;
  socklen_t len;
  n = recvfrom(sock1, (char*)buffer, 512, MSG_WAITALL, (struct sockaddr*)&name,
               &len);
  buffer[n] = '\0';
  printf("Server : %s\n", buffer);

  close(sock1);
}

int main(int argc, char* argv[]) {
  unix_udp_test();

  return 0;
}
