#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}

int main(int argc, char* argv[]) {
  if (argc < 5)
    return create_error("Please provite 4 arguments");

  char* path = argv[1];

  char* end;

  errno = 0;
  end = NULL;
  int pmin = (int)strtol(argv[2], &end, 10);
  if (errno != 0 || end == argv[2])
    return create_error("Second argument is not a number");

  errno = 0;
  end = NULL;
  int pmax = (int)strtol(argv[3], &end, 10);
  if (errno != 0 || end == argv[3])
    return create_error("Third argument is not a number");

  errno = 0;
  end = NULL;
  int bytes = (int)strtol(argv[4], &end, 10);
  if (errno != 0 || end == argv[4])
    return create_error("Fourth argument is not a number");

  if (pmax < 1)
    return create_error("Pmax cannot be less than 1");
  if (pmin < 0)
    return create_error("Pmin cannot be less than 0");
  if (pmax < pmin)
    return create_error("Pmax cannot be less than Pmin");
  if (bytes < 0)
    return create_error("Bytes cannot be less than 0");

  srand(time(NULL));

  FILE* fd;
  if ((fd = fopen(path, "a")) == NULL)
    return create_error("Cannot open file");

  char* buffer = malloc(255);
  char* date_buffer = malloc(255);
  char* random_buffer = malloc(bytes + 1);
  random_buffer[bytes] = '\0';
  time_t rawtime;
  struct tm* timeinfo;

  while (1) {
    int sec = (rand() % (pmax - pmin + 1)) + pmin;
    sleep(sec);

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date_buffer, 50, "%Y-%m-%d %H:%M:%S", timeinfo);

    for (int i = 0; i < bytes; i++) {
      random_buffer[i] = 'A' + (rand() % 26);
    }

    snprintf(buffer, 255, "\nPID: %d SEC: %d DATE: %s DATA: %s", getpid(), sec,
             date_buffer, random_buffer);

    if (fwrite(buffer, 1, strlen(buffer), fd) != strlen(buffer)) {
      free(buffer);
      free(date_buffer);
      free(random_buffer);
      fclose(fd);
      return create_error("Cannot write to file");
    }
    fflush(fd);
  }

  free(buffer);
  free(date_buffer);
  free(random_buffer);
  fclose(fd);
}