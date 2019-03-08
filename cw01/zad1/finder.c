#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "finder.h"

const int COMMAND_BUFFER_SIZE = 255;

struct search_params
{
    char *dir;
    char *file;
    char *name_file_temp;

};

int library_initalized = 0;

struct search_params *main_search_params;
char **search_results;
int search_results_size;


int check_library_initalized()
{
    if(library_initalized == 0){
        fprintf(stderr, "libfinder - not initalized\n");
        return -1;
    }
    return 0;
}

int create_table(unsigned int table_size)
{
    if(library_initalized == 1){
        fprintf(stderr, "libfinder - already initalized\n");
        return -1;
    }

    main_search_params = malloc(sizeof(struct search_params));
    search_results = calloc(table_size, sizeof(char*));
    search_results_size = table_size;
    library_initalized = 1;

    return 0;
}

int set_search(char *dir, char *file, char *name_file_temp)
{
    if(check_library_initalized() < 0)
        return -1;

    main_search_params->dir = dir;
    main_search_params->file = file;
    main_search_params->name_file_temp = name_file_temp;

    return 0;
}

int store_last_result()
{
    if(check_library_initalized() < 0)
        return -1;

    int fd = open(main_search_params->name_file_temp, O_RDONLY);
    int start_position = lseek(fd, 0, SEEK_CUR);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, start_position, SEEK_SET);
    char *file_buffer = calloc(size, sizeof(char));
    read(fd, file_buffer, size);
    close(fd);
    

    for(int i = 0; i < search_results_size; i++)
    {
        if(search_results[i] == NULL) 
        {
            search_results[i] = file_buffer;
            return i;
        }
    }

    fprintf(stderr, "libfinder - cannot add more new data blocks\n");
    return -1;
}

int search_directory()
{
    if(check_library_initalized() < 0)
        return -1;

    char buffer[COMMAND_BUFFER_SIZE];
    if(snprintf(buffer, COMMAND_BUFFER_SIZE, "find %s -name %s > %s 2>/dev/null",
        main_search_params->dir,
        main_search_params->file,
        main_search_params->name_file_temp)
        >=sizeof(buffer))
    {
        printf("Error");
    } else 
    {
        system(buffer);
    }
    return 0;
}

int search_directory_and_store()
{
    if(check_library_initalized() < 0)
        return -1;

    search_directory();
    return store_last_result();
}

int remove_data_block(int index)
{
    if(check_library_initalized() < 0)
        return -1;

    if(search_results[index] != NULL) {
        free(search_results[index]);
        search_results[index] = NULL;
        return 0;
    }
    fprintf(stderr, "libfinder - there is no datablock with index %d\n", index);
    return -1;
}






