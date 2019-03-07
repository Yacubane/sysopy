#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFFER_SIZE = 255;

struct search_params
{
    char *dir;
    char *file;
    char *name_file_temp;

};

struct search_result
{
    char *result;
};

struct search_params *main_search_params;
struct search_result **search_results;


void
create_table(unsigned int table_size)
{
    main_search_params = malloc(sizeof(struct search_params));
    search_results = calloc(sizeof(struct search_result*), table_size);

   // search_results[0] = malloc(sizeof(struct search_result));

    printf("%p", search_results[0]);
    printf("%p", search_results[1]);

}

void
set_search(char *dir, char *file, char *name_file_temp)
{
    main_search_params->dir = dir;
    main_search_params->file = file;
    main_search_params->name_file_temp = name_file_temp;
}

void
search_directory()
{
    char buffer[BUFFER_SIZE];
    if(snprintf(buffer, BUFFER_SIZE, "find %s -name %s > %s",
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

    int fd = open(main_search_params->name_file_temp, O_RDONLY);
    int start_position = lseek(fd, 0, SEEK_CUR);
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, start_position, SEEK_SET);
    char *file_buffer = malloc(sizeof(char)*size);
    read(fd, file_buffer, size);
    close(fd);

    
}


int
main(int argc, char *argv[])
{
    create_table(100);
    set_search("~/Studia", "prog.c", "tmp.txt");
    search_directory();
    return 0;
}



