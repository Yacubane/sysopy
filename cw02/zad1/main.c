#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "reporting.h"

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}

int check_argument_size(int index, int arguments_length, int argc) {
  if (index + arguments_length >= argc) {
    fprintf(stderr, "Wrong number of arguments\n");
    return -1;
  }
  return 0;
}

int parse_command(int index, int argc, char* argv[]) {
  if (!strcmp(argv[index], "generate")) {
    if (check_argument_size(index, 3, argc) < 0)
      return create_error("Error parsing generate, wrong arguments size");

    char* file_name = argv[index + 1];

    errno = 0;
    char* end = NULL;
    int record_num = (int)strtol(argv[index + 2], &end, 10);

    if (errno != 0 || end == argv[index + 2])
      return create_error(
          "Error parsing generate, second argument must be a number");

    errno = 0;
    end = NULL;
    int record_size = (int)strtol(argv[index + 3], &end, 10);

    if (errno != 0 || end == argv[index + 3])
      return create_error(
          "Error parsing generate, third argument must be a number");

    generate(file_name, record_num, record_size);
    return index + 3;
  } else if (!strcmp(argv[index], "sort")) {
    if (check_argument_size(index, 4, argc) < 0)
      return create_error("Error parsing sort, wrong arguments size");

    char* file_name = argv[index + 1];

    errno = 0;
    char* end = NULL;
    int record_num = (int)strtol(argv[index + 2], &end, 10);

    if (errno != 0 || end == argv[index + 2])
      return create_error(
          "Error parsing sort, second argument must be a number");

    errno = 0;
    end = NULL;
    int record_size = (int)strtol(argv[index + 3], &end, 10);

    if (errno != 0 || end == argv[index + 3])
      return create_error(
          "Error parsing sort, third argument must be a number");

    char* lib_flag_string = argv[index + 4];
    int lib_flag = 0;
    if (!strcmp(lib_flag_string, "sys"))
      lib_flag = FILES_SYS;
    else if (!strcmp(lib_flag_string, "lib"))
      lib_flag = FILES_LIB;
    else
      return create_error(
          "Error parsing sort, fourth argument must be \"sys\" or \"lib\"");

    start_reporting();
    start_report_timer();
    sort(file_name, record_num, record_size, lib_flag);
    stop_report_timer();
    return index + 4;
  } else if (!strcmp(argv[index], "copy")) {
    if (check_argument_size(index, 5, argc) < 0)
      return create_error("Error parsing copy, wrong arguments size");

    char* file_name1 = argv[index + 1];
    char* file_name2 = argv[index + 2];

    errno = 0;
    char* end = NULL;
    int record_num = (int)strtol(argv[index + 3], &end, 10);

    if (errno != 0 || end == argv[index + 3])
      return create_error(
          "Error parsing copy, third argument must be a number");

    errno = 0;
    end = NULL;
    int record_size = (int)strtol(argv[index + 4], &end, 10);

    if (errno != 0 || end == argv[index + 4])
      return create_error(
          "Error parsing copy, fourth argument must be a number");

    char* lib_flag_string = argv[index + 5];
    int lib_flag = 0;
    if (!strcmp(lib_flag_string, "sys"))
      lib_flag = FILES_SYS;
    else if (!strcmp(lib_flag_string, "lib"))
      lib_flag = FILES_LIB;
    else
      return create_error(
          "Error parsing sort, fourth argument must be \"sys\" or \"lib\"");

    start_reporting();
    start_report_timer();
    copy(file_name1, file_name2, record_num, record_size, lib_flag);
    stop_report_timer();
    return index + 5;
  } else {
    fprintf(stderr, "Error parsing %s - undefined command\n", argv[index]);
    return -1;
  }
}

int main(int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    int parse_result = parse_command(i, argc, argv);
    if (parse_result < 0) {
      fprintf(stderr, "Parse error, stopping\n");
      return -1;
    }
    i = parse_result;
  }
  return 0;
}