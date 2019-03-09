#include "finder_dynamic.h"
#include <dlfcn.h>
#include <stdio.h>

int (*create_table_)(int table_size);
int (*set_search_)(char *dir, char *file, char *name_file_temp);
int (*search_directory_)();
int (*search_directory_and_store_)();
int (*store_last_result_)();
int (*remove_data_block_)(int index);
char* (*get_data_block_)(int index);

int create_error(char* message) {
    fprintf(stderr, "%s", message);
    return -1;
}

int init_dynamic()
{
    void *handle = dlopen("../zad1/libfinder.so", RTLD_LAZY);
    if(!handle){
        fprintf(stderr, "Cannot load dynamic library");
        return -1;
    }

    create_table_ = dlsym(handle, "create_table");
    if(dlerror() != NULL) return create_error("Cannot load create_table\n");

    set_search_ = dlsym(handle, "set_search");
    if(dlerror() != NULL) return create_error("Cannot load set_search\n");

    search_directory_ = dlsym(handle, "search_directory");
    if(dlerror() != NULL) return create_error("Cannot load search_directory\n");

    search_directory_and_store_ = dlsym(handle, "search_directory_and_store");
    if(dlerror() != NULL) return create_error("Cannot load search_directory_and_store\n");

    store_last_result_ = dlsym(handle, "store_last_result");
    if(dlerror() != NULL) return create_error("Cannot load store_last_result\n");

    remove_data_block_ = dlsym(handle, "remove_data_block");
    if(dlerror() != NULL) return create_error("Cannot load remove_data_block\n");

    get_data_block_ = dlsym(handle, "get_data_block");
    if(dlerror() != NULL) return create_error("Cannot load get_data_block\n");

    return 0;
}

int create_table(unsigned int table_size) {
    return (*create_table_)(table_size);
}
int set_search(char *dir, char *file, char *name_file_temp)
{
    return (*set_search_)(dir, file, name_file_temp);
}
int search_directory()
{
    return (*search_directory_)();
}
int search_directory_and_store()
{
    return (*search_directory_and_store_)();
}
int store_last_result()
{
    return (*store_last_result_)();
}
int remove_data_block(int index)
{
    return (*remove_data_block_)(index);
}
char* get_data_block(int index)
{
    return (*get_data_block_)(index);
}