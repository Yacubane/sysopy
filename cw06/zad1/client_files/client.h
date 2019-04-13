#pragma once

void send_echo(char *message);
void send_list();
void send_friends(int *friends, int friends_size, int array_size);
void send_add(int id);
void send_del(int id);
void send_to_all(char* message);
void send_to_friends(char* message);
void send_to_one(int id, char* message);
void send_stop();