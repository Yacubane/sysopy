#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define TYPE_KILL 0
#define TYPE_SIGQUEUE 1
#define TYPE_SIGRT 2

int signal_counter = 0;
int type = 0;

static int create_error(char *message) 
{
    fprintf(stderr, "%s\n", message);
    return -1;
}
void handleSIGUSR (int signo, siginfo_t* info, void* ptr) {
  if (signo == SIGUSR1 || signo == SIGRTMIN) {
      signal_counter++;
   } else if (signo == SIGUSR2 || signo == SIGRTMIN+1) {
        pid_t pid = info->si_pid;
        if(type == TYPE_KILL) {
            for(int i = 0; i < signal_counter; i++) 
                kill(pid, SIGUSR1);
            kill(pid, SIGUSR2);
        } else if(type == TYPE_SIGQUEUE) {
            for(int i = 0; i < signal_counter; i++) {
                union sigval val;
                val.sival_int = i+1;
                sigqueue(pid, SIGUSR1, val);
            }
            union sigval val;
            sigqueue(pid, SIGUSR2, val);
        } else if(type == TYPE_SIGRT) {
            for(int i = 0; i < signal_counter; i++) 
                kill(pid, SIGRTMIN);
            kill(pid, SIGRTMIN+1);
        }

       printf("Catcher read %d signals\n", signal_counter);
       exit(0);
   }
}

int main(int argc, char* argv[])
{
    signal_counter = 0;

    if(argc != 2)
        return create_error("Please provide 1 argument");

    type = 0;
    if (strcmp(argv[1], "KILL") == 0)
        type = TYPE_KILL;
    else if (strcmp(argv[1], "SIGQUEUE") == 0)
        type = TYPE_SIGQUEUE;
    else if (strcmp(argv[1], "SIGRT") == 0)
        type = TYPE_SIGRT;
    else return create_error("Third argument must be \"KILL\", \"SIGQUEUE\" or \"SIGRT\"");   

    sigset_t newmask;
    sigset_t oldmask; 
    sigemptyset(&newmask);
    sigfillset(&newmask); 

    if(type == 0 || type == 1) {
        sigdelset(&newmask, SIGUSR1); 
        sigdelset(&newmask, SIGUSR2); 
    } else if(type == 2) {
        sigdelset(&newmask, SIGRTMIN); 
        sigdelset(&newmask, SIGRTMIN+1); 
    }

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        perror("Cannot block signal");

    struct sigaction action;
    action.sa_flags=SA_SIGINFO; //dla sa_sigaction a nie sa_handler
    action.sa_sigaction = handleSIGUSR;
    sigemptyset(&action.sa_mask); 

    if(type == TYPE_KILL || type == TYPE_SIGQUEUE) {
        sigaddset(&action.sa_mask, SIGUSR1); 
        sigaddset(&action.sa_mask, SIGUSR2); 
        sigaction(SIGUSR1, &action, NULL);
        sigaction(SIGUSR2, &action, NULL);

    }else if(type == TYPE_SIGRT) {
        sigaddset(&action.sa_mask, SIGRTMIN); 
        sigaddset(&action.sa_mask, SIGRTMIN+1); 
        sigaction(SIGRTMIN, &action, NULL);
        sigaction(SIGRTMIN+1, &action, NULL);
    }


    printf("Catcher PID: %d\n", getpid());
    while(1);

}