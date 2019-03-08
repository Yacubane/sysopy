#pragma once

void create_table(unsigned int table_size);
void set_search(char *dir, char *file, char *name_file_temp);
void search_directory();
int search_directory_and_save();
int save_last_result();
int remove_data_block(int index);