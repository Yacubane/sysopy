#pragma once

int create_table(unsigned int table_size);
int set_search(char *dir, char *file, char *name_file_temp);
int search_directory();
int search_directory_and_store();
int store_last_result();
int remove_data_block(int index);
char* get_data_block(int index);