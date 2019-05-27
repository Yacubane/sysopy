#include "cparser.h"
#include <stdio.h>
#include "../shared_files/global_msgtypes.h"

int starts_with(char* string, char* start) {
  if (strncmp(string, start, strlen(start)) == 0)
    return 1;
  return 0;
}

void send_echo(char* message) {
  send_message(TYPE_ECHO, message);
}
void send_list() {
  send_message(TYPE_LIST, "");
}
void send_friends(int* friends, int friends_size, int array_size) {
  char buffer[255];
  char num_buffer[10];
  strcpy(buffer, "");
  for (int i = 0; i < friends_size; i++) {
    snprintf(num_buffer, 10, "%d", friends[i]);
    if (i < (friends_size - 1))
      strcat(num_buffer, " ");
    strcat(buffer, num_buffer);
  }

  send_message(TYPE_FRIENDS, buffer);
}
void send_add(int id) {
  char buffer[255];
  snprintf(buffer, 255, "%d", id);
  send_message(TYPE_ADD, buffer);
}
void send_del(int id) {
  char buffer[255];
  snprintf(buffer, 255, "%d", id);
  send_message(TYPE_DEL, buffer);
}
void send_to_all(char* message) {
  send_message(TYPE_2ALL, message);
}
void send_to_friends(char* message) {
  send_message(TYPE_2FRIENDS, message);
}
void send_to_one(int id, char* message) {
  char buffer[255];
  snprintf(buffer, 255, "%d %s", id, message);
  send_message(TYPE_2ONE, buffer);
}

void parse_command(char* in_buffer, char* out_buffer, int size) {
  if (starts_with(in_buffer, "ECHO")) {
    sscanf(in_buffer + strlen("ECHO"), "%s", out_buffer);
    send_echo(out_buffer);
  } else if (starts_with(in_buffer, "LIST")) {
    send_list();
  } else if (starts_with(in_buffer, "FRIENDS")) {
    int friends_no = 0;
    for (int i = 0; i < strlen(in_buffer); i++) {
      if (in_buffer[i] == ' ')
        friends_no++;
    }
    int friends[friends_no];

    char* ptr = strtok(in_buffer + strlen("FRIENDS"), " \n");
    int i = 0;
    while (ptr != NULL) {
      int friend_id;
      if (parse_num(ptr, &friend_id) < 0)
        outerr("Cannot parse friend id");
      else
        friends[i++] = friend_id;
      ptr = strtok(NULL, " \n");
    }
    send_friends(friends, i, friends_no);
  } else if (starts_with(in_buffer, "ADD")) {
    int id;
    if (sscanf(in_buffer + strlen("ADD"), "%d", &id) < 1) {
      outerr("ADD must have one numeric argument");
      return;
    }
    send_add(id);
  } else if (starts_with(in_buffer, "DEL")) {
    int id;
    if (sscanf(in_buffer + strlen("DEL"), "%d", &id) < 1) {
      outerr("DEL must have one numeric argument");
      return;
    }
    send_del(id);
  } else if (starts_with(in_buffer, "2ALL")) {
    sscanf(in_buffer + strlen("2ALL") + 1, "%[^\t\n]", out_buffer);
    send_to_all(out_buffer);
  } else if (starts_with(in_buffer, "2FRIENDS")) {
    sscanf(in_buffer + strlen("2FRIENDS") + 1, "%[^\t\n]", out_buffer);
    send_to_friends(out_buffer);
  } else if (starts_with(in_buffer, "2ONE")) {
    int id;
    sscanf(in_buffer + strlen("2ONE") + 1, "%d %[^\t\n]", &id, out_buffer);
    send_to_one(id, out_buffer);
  } else if (starts_with(in_buffer, "READ")) {
    FILE* fd;
    if ((fd = fopen(in_buffer + 5, "r")) == NULL) {
      outerr("Cannot open list file");
      return;
    }
    char buffer[255];
    while (fgets(buffer, 255, fd) != NULL) {
      int length = strlen(buffer);
      if (length > 1 && buffer[length - 1] == '\n')
        buffer[length - 1] = '\0';

      printf("%s\n", buffer);
      parse_command(buffer, out_buffer, size);
    }
    fclose(fd);
  } else if (starts_with(in_buffer, "STOP")) {
    stop_client();
  } else {
    outerr("Unknown command");
  }
}

void run_cparser() {
  char in_buffer[255];
  char out_buffer[255];
  while (1) {
    fgets(in_buffer, 255, stdin);
    int length = strlen(in_buffer);
    if (length > 1 && in_buffer[length - 1] == '\n')
      in_buffer[length - 1] = '\0';
    parse_command(in_buffer, out_buffer, 255);
  }
}