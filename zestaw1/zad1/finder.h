#pragma once

void create_table(unsigned int table_size);
void set_search(char *dir, char *file, char *name_file_temp);
int search_directory();
int remove_data_block(int index);