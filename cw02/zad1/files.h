#pragma once

#define FILES_SYS 0
#define FILES_LIB 1

int generate(char* filename, int record_num, int record_size);
int sort(char* filename, int record_num, int record_size, int lib_type);
int copy(char* filename1,
         char* filename2,
         int record_num,
         int record_size,
         int lib_flag);
