#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return 1;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));

  if (argc != 3)
    return create_error("Please provide 2 arguments (path, count)");

  char* end;
  errno = 0;
  end = NULL;
  int count = (int)strtol(argv[2], &end, 10);
  if (errno != 0 || end == argv[2])
    return create_error("Second argument is not a number");

  int fd;
  if ((fd = open(argv[1], O_WRONLY)) < 0)
    return create_error("Cannot open file");

  printf("Started slave with PID: %d\n", getpid());

  int buff_size = 255;
  char buff[buff_size];
  int date_buff_size = 127;
  char date_buff[127];
  for (int i = 0; i < count; i++) {
    FILE* file = popen("date", "r");
    fread(date_buff, sizeof(char), date_buff_size, file);
    pclose(file);

    date_buff[strlen(date_buff) - 1] = '\0';
    snprintf(buff, buff_size, "PID: %d DATE: %s", getpid(), date_buff);
    write(fd, buff, buff_size);
    sleep(2 + rand() % 3);
  }
}