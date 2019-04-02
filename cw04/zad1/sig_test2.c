#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

pid_t fork_pid;

pid_t create_fork() 
{
    fork_pid = fork();
    if(fork_pid == 0) {
        char buffer[255];
        realpath("script.sh", buffer);
        execlp("/bin/sh", "/bin/sh", buffer, NULL);
        exit(0);
    } else {
        return fork_pid;
    }
}

void SIGINThandler(int signum){
    printf("Odebrano sygnał SIGINT\n");
    if(fork_pid > 0)
        kill(fork_pid, SIGKILL);
    exit(0);
}

void SIGTSTPhandler(int sig_no) 
{
    if(fork_pid > 0) {
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakończenie programu\n");
        kill(fork_pid, SIGKILL);
        fork_pid = 0;
    } else {
        create_fork();
    }
} 

int main(int argc, char *argv[])
{
    fork_pid = 0;
    create_fork();

    signal(SIGINT, SIGINThandler);

    struct sigaction act; 
    act.sa_handler = SIGTSTPhandler; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 
    sigaction(SIGTSTP, &act, NULL); 
    
    while (1) sleep(1);
}