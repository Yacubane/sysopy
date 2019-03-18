#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "files.h"
#include <limits.h>
#include <time.h>

static int create_error(char* message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}

static int create_error_and_close(int fd, char* message) 
{
    close(fd);
    fprintf(stderr, "%s\n", message);
    return -1;
}
static int create_error_and_closef(FILE* fd, char* message) 
{
    fclose(fd);
    fprintf(stderr, "%s\n", message);
    return -1;
}

int generate(char* filename, int record_num, int record_size)
{
    srand(time(NULL));
    int fd;
    if((fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0644)) >= 0) 
    {
        char* buffer = calloc(record_size, sizeof(char));
        for(int i = 0; i < record_num; i++) 
        {
            for(int i = 0; i < record_size; i++) 
            {
                buffer[i] = rand() % 127;
            }
            if(write(fd, buffer, record_size) != record_size) 
                return create_error_and_close(fd, "files - error during writing to file");
            
        }
        free(buffer);
    } else 
    {
        return create_error("files - cannot open file");
    }
    close(fd);
    return 0;
}
int sort(char *filename, int record_num, int record_size, int lib_flag)
{
    if(lib_flag == 0) 
    {
        int fd;
        if((fd = open(filename, O_RDWR)) < 0) 
            return create_error("files - cannot open file");
        
        char *i_buffer = calloc(record_size, sizeof(char));
        char *min_buffer = calloc(record_size, sizeof(char));
        for(int i = 0; i < record_num; i++) 
        {
            int min_index = i;
            unsigned char min_value = UCHAR_MAX;
            for(int j = min_index; j < record_num; j++)
            {

                if(lseek(fd, record_size * j, SEEK_SET) < 0) 
                    return create_error_and_close(fd, "files - cannot seek file");  
                
                unsigned char first;
                if(read(fd, &first, 1) != 1)
                    return create_error_and_close(fd, "files - cannot read first char");  

                if(first < min_value) 
                {
                    min_index = j;
                    min_value = first;
                }

            }

            if(min_index != i) {
                if(lseek(fd, record_size * i, SEEK_SET) < 0) 
                    return create_error_and_close(fd, "files - cannot seek file");
                if(read(fd, i_buffer, record_size) != record_size)
                    return create_error_and_close(fd, "files - cannot read file");

                if(lseek(fd, record_size * min_index, SEEK_SET) < 0) 
                    return create_error_and_close(fd, "files - cannot seek file");
                if(read(fd, min_buffer, record_size) != record_size)
                    return create_error_and_close(fd, "files - cannot read file");

                if(lseek(fd, record_size * min_index, SEEK_SET) < 0) 
                    return create_error_and_close(fd, "files - cannot seek file");  
                if(write(fd, i_buffer, record_size) != record_size)
                    return create_error_and_close(fd, "files - cannot write to file");  

                if(lseek(fd, record_size * i, SEEK_SET) < 0) 
                    return create_error_and_close(fd, "files - cannot seek file");  
                if(write(fd, min_buffer, record_size) != record_size)
                    return create_error_and_close(fd, "files - cannot write to file"); 
            }
        }
        close(fd);
        

    } else if(lib_flag == 1)
    {
        FILE* fd;
        if((fd = fopen(filename, "r+b")) == NULL) 
            return create_error("files - cannot open file");
        
        char *i_buffer = calloc(record_size, sizeof(char));
        char *min_buffer = calloc(record_size, sizeof(char));
        for(int i = 0; i < record_num; i++) 
        {
            int min_index = i;
            unsigned char min_value = UCHAR_MAX;
            for(int j = min_index; j < record_num; j++)
            {

                if(fseek(fd, record_size * j, 0) != 0) 
                    return create_error_and_closef(fd, "files - cannot seek file");  
                
                unsigned char first;
                if(fread(&first, 1, 1, fd) != 1) 
                    return create_error_and_closef(fd, "files - cannot read first char");  

                if(first < min_value) 
                {
                    min_index = j;
                    min_value = first;
                }

            }

            if(min_index != i) {
                if(fseek(fd, record_size * i, 0)  != 0) 
                    return create_error_and_closef(fd, "files - cannot seek file");
                if(fread(i_buffer, 1, record_size, fd) != record_size)
                    return create_error_and_closef(fd, "files - cannot read file");

                if(fseek(fd, record_size * min_index, 0)  != 0) 
                    return create_error_and_closef(fd, "files - cannot seek file");
                if(fread(min_buffer, 1, record_size, fd) != record_size)
                    return create_error_and_closef(fd, "files - cannot read file");

                if(fseek(fd, record_size * min_index, 0)  != 0) 
                    return create_error_and_closef(fd, "files - cannot seek file");  
                if(fwrite(i_buffer, 1, record_size, fd) != record_size)
                    return create_error_and_closef(fd, "files - cannot write to file");  

                if(fseek(fd, record_size * i, 0)  != 0) 
                    return create_error_and_closef(fd, "files - cannot seek file");  
                if(fwrite(min_buffer, 1, record_size, fd) != record_size)
                    return create_error_and_closef(fd, "files - cannot write to file"); 
            }
        }
        fclose(fd);
    } else
    {
        return create_error("files - unknown lib flag");
    }
    return 0;
}
int copy(char *filename1, char *filename2, int record_num, int record_size, int lib_flag)
{
    if(lib_flag == 0) 
    {
        int fd1;
        if((fd1 = open(filename1, O_RDONLY)) < 0) 
            return create_error("files - error during opening file1");

        int fd2;
        if((fd2 = open(filename2, O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) 
            return create_error("files - error during creating file2");

        char* buffer = calloc(record_size, sizeof(char));
        for(int i = 0; i < record_num; i++) {
            int read_result = read(fd1, buffer, record_size);
            if(read_result != record_size && read_result != 0) {
                close(fd1);
                close(fd2);
                return create_error("files - file 1 is not multiciplity of record size");
            }
            if(write(fd2, buffer, record_size) != record_size) {
                close(fd1);
                close(fd2);
                return create_error("files - cannot write to file 2");
            }
        }
        free(buffer);
        close(fd1);
        close(fd2);
    } else if(lib_flag == 1) 
    {
        FILE* fd1;
        if((fd1 = fopen(filename1, "r")) == NULL) 
            return create_error("files - error during opening file1");

        FILE* fd2;
        if((fd2 = fopen(filename2, "w")) == NULL) 
            return create_error("files - error during creating file2");


        char* buffer = calloc(record_size, sizeof(char));
        for(int i = 0; i < record_num; i++) {
            int read_result = fread(buffer, 1, record_size, fd1);

            if(read_result != record_size && read_result != 0) {
                fclose(fd1);
                fclose(fd2);
                return create_error("files - file 1 is not multiciplity of record size");
            }

            if(fwrite(buffer, 1, record_size, fd2) != record_size) {
                fclose(fd1);
                fclose(fd2);
                return create_error("files - cannot write to file 2");
            }
        }
        free(buffer);
        fclose(fd1);
        fclose(fd2);

    } else 
    {
        return create_error("files - unknown lib flag");
    }

    return 0;
}
