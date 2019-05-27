#include "client_helper.h"
#include <stdio.h>
#include <string.h>

void print_server_response(char* message) {
  if (strlen(message) >= 5 && strncmp(message, "ERROR", 5) == 0)
    set_color(ANSI_COLOR_RED);
  else if (strlen(message) >= 7 && strncmp(message, "SUCCESS", 7) == 0)
    set_color(ANSI_COLOR_GREEN);
  else if (strlen(message) >= 7 && strncmp(message, "WARNING", 7) == 0)
    set_color(ANSI_COLOR_YELLOW);
  else
    set_color(ANSI_COLOR_CYAN);
  printf("SERVER: %s", message);
  reset_color();
}