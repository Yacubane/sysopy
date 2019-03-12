#include "finder.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct search_instance_t
{
    char *dir;
    char *file;
    char *name_file_temp;

    char **search_results;
    unsigned int search_results_size;

};

static int library_initalized = 0;

static struct search_instance_t *search_instance;

static int create_error(char *message)
{
    fprintf(stderr, "%s\n", message);
    return -1;
}

int check_library_initalized()
{
    if(library_initalized == 0)
        return create_error("libfinder - not initalized");

    return 0;
}

int create_table(unsigned int table_size)
{
    if(library_initalized == 1)
        return create_error("libfinder - already initalized");


    search_instance = malloc(sizeof(struct search_instance_t));
    search_instance->search_results = calloc(table_size, sizeof(char*));
    search_instance->search_results_size = table_size;
    library_initalized = 1;

    return 0;
}

int set_search(char *dir, char *file, char *name_file_temp)
{
    if(check_library_initalized() < 0)
        return -1;

    search_instance->dir = dir;
    search_instance->file = file;
    search_instance->name_file_temp = name_file_temp;
    library_initalized = 2;

    return 0;
}

int search_directory()
{
    if(check_library_initalized() < 0)
        return -1;

    if(check_library_initalized() == 1)
        return create_error("libfinder - you haven't specified search parameters");


    int buffer_size = 40 + strlen(search_instance->dir) + 
                strlen(search_instance->file)+
                strlen(search_instance->name_file_temp);

    char buffer[buffer_size];

    snprintf(buffer, buffer_size, "find %s -name %s > %s 2>/dev/null",
        search_instance->dir,
        search_instance->file,
        search_instance->name_file_temp);

    system(buffer);

    return 0;
}

int store_last_result()
{
    if(check_library_initalized() < 0)
        return -1;

    int fd;
    if((fd = open(search_instance->name_file_temp, O_RDONLY)) < 0)
        return create_error("libfinder - error during reading tmp file");

    int start_position = lseek(fd, 0, SEEK_CUR);
    int size;
    if((size = lseek(fd, 0, SEEK_END)) < 0) {
        close(fd);
        return create_error("libfinder - cannot seek tmp file");
    }

    lseek(fd, start_position, SEEK_SET);
    char *file_buffer = calloc(size, sizeof(char));
    if(read(fd, file_buffer, size) < 0){
        close(fd);
        return create_error("libfinder - cannot read tmp file");
    }
    close(fd);

    for(int i = 0; i < search_instance->search_results_size; i++)
    {
        if(search_instance->search_results[i] == NULL) 
        {
            search_instance->search_results[i] = file_buffer;
            return i;
        }
    }
    return create_error("libfinder - cannot add more new data blocks");
}

int search_directory_and_store()
{
    if(search_directory() < 0)
        return -1;
    return store_last_result();
}


int remove_data_block(int index)
{
    if(check_library_initalized() < 0)
        return -1;

    if(search_instance->search_results[index] != NULL) 
    {
        free(search_instance->search_results[index]);
        search_instance->search_results[index] = NULL;
        return 0;
    }
    fprintf(stderr, "libfinder - there is no datablock with index %d\n", index);
    return -1;
}

char* get_data_block(int index) 
{
    if(check_library_initalized() < 0)
        return NULL;

    if(search_instance->search_results[index] != NULL) 
    {
        return search_instance->search_results[index];
    }
    else {
        fprintf(stderr, "libfinder - there is no datablock with index %d\n", index);
        return NULL;
    }
}

int clean() 
{
    if(check_library_initalized() < 0)
        return create_error("libfinder - cannot clean uninitalized library");

    for(int i = 0; i < search_instance->search_results_size; i++)
    {
        if(search_instance->search_results[i] != NULL) 
        {
            free(search_instance->search_results[i]);
        }
    }
    free(search_instance->search_results);
    free(search_instance);
    library_initalized = 0;

    return 0;
}




