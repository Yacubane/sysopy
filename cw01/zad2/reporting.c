#include "reporting.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int reporting_status = 0;
int fd;

int start_reporting(char* filename)
{
    if(reporting_status==1)
        return -2;
    

    if((fd = creat(filename, 0644)) >= 0) 
    {
        reporting_status = 1;
        return 0;
    } else
        return -1;
    
}

int add_report_text(char* content) {
    if(reporting_status==0)
        return -2;
    

    int string_size = strlen(content);
    if(write(fd, content, string_size) != string_size) {
        reporting_status = 0;
        close(fd);
        return -1;
    }
    return 0;

}

int stop_reporting() {
    if(reporting_status==0)
        return -1;

    reporting_status = 0;
    close(fd);

    return 0;    
}
