#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return 1;
}

int main(int argc, char* argv[]) {
  if (argc != 2)
    return create_error("Please provide 1 argument (path)");

  if (mkfifo(argv[1], 0666) < 0)
    return create_error("Cannot make fifo");

  int fd;
  if ((fd = open(argv[1], O_RDONLY)) < 0)
    return create_error("Cannot open file");

  int buff_size = 255;
  char buff[buff_size];
  while (1) {
    int r = read(fd, buff, buff_size);
    if (r < 0)
      return create_error("Error when reading");
    else if (r > 0)
      printf("%s\n", buff);
  }
}