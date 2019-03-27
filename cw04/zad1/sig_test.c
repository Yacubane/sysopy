#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int run;
void SIGINThandler(int signum){
    printf("Odebrano sygnał SIGINT\n");
    exit(0);
}

void au(int sig_no) 
{
    if(run == 1) {
        run = 0;
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakończenie programu\n");
    } else {
        run=1;
    }
} 

int main(int argc, char *argv[])
{
    signal(SIGINT, SIGINThandler);

    struct sigaction act; 
    act.sa_handler = au; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 
    sigaction(SIGTSTP, &act, NULL); 
    
    run = 1;

    while(1) {
        sleep(1);
        if(run) {
            char date_buffer[50];
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime (date_buffer,50,"%Y-%m-%d %H:%M:%S\n",timeinfo);
            printf("%s", date_buffer);
        }
 
    }

}