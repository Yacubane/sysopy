#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TYPE_KILL 0
#define TYPE_SIGQUEUE 1
#define TYPE_SIGRT 2

int signal_counter;
int signals_num;
int type;

static int create_error(char* message) {
  fprintf(stderr, "%s\n", message);
  return -1;
}

void handleSignals(int signo, siginfo_t* info, void* context) {
  if (signo == SIGUSR1 || signo == SIGRTMIN) {
    signal_counter++;
    if (type == TYPE_SIGQUEUE) {
      printf("Sender is reading signal %d and this is %d signal from catcher\n",
             signal_counter, info->si_value.sival_int);
    }
  } else if (signo == SIGUSR2 || signo == SIGRTMIN + 1) {
    printf("Sender send %d and read %d signals\n", signals_num, signal_counter);
    exit(0);
  }
}

int main(int argc, char* argv[]) {
  signal_counter = 0;

  if (argc != 4)
    return create_error("Please provide 3 arguments");

  char* end;

  errno = 0;
  end = NULL;
  int catcher_pid = (int)strtol(argv[1], &end, 10);
  if (errno != 0 || end == argv[1])
    return create_error("First argument is not a number");

  errno = 0;
  end = NULL;
  signals_num = (int)strtol(argv[2], &end, 10);
  if (errno != 0 || end == argv[2])
    return create_error("Second argument is not a number");

  type = 0;
  if (strcmp(argv[3], "KILL") == 0)
    type = TYPE_KILL;
  else if (strcmp(argv[3], "SIGQUEUE") == 0)
    type = TYPE_SIGQUEUE;
  else if (strcmp(argv[3], "SIGRT") == 0)
    type = TYPE_SIGRT;
  else
    return create_error(
        "Third argument must be \"KILL\", \"SIGQUEUE\" or \"SIGRT\"");

  sigset_t newmask;
  sigset_t oldmask;
  sigemptyset(&newmask);
  sigfillset(&newmask);

  if (type == 0 || type == 1) {
    sigdelset(&newmask, SIGUSR1);
    sigdelset(&newmask, SIGUSR2);
  } else if (type == 2) {
    sigdelset(&newmask, SIGRTMIN);
    sigdelset(&newmask, SIGRTMIN + 1);
  }

  if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    perror("Cannot block signal");

  struct sigaction action;
  action.sa_flags = SA_SIGINFO;  // dla sa_sigaction a nie sa_handler
  action.sa_sigaction = handleSignals;
  sigemptyset(&action.sa_mask);
  if (type == TYPE_KILL || type == TYPE_SIGQUEUE) {
    sigaddset(&action.sa_mask, SIGUSR1);
    sigaddset(&action.sa_mask, SIGUSR2);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

  } else if (type == TYPE_SIGRT) {
    sigaddset(&action.sa_mask, SIGRTMIN);
    sigaddset(&action.sa_mask, SIGRTMIN + 1);
    sigaction(SIGRTMIN, &action, NULL);
    sigaction(SIGRTMIN + 1, &action, NULL);
  }

  if (type == TYPE_KILL) {
    for (int i = 0; i < signals_num; i++)
      kill(catcher_pid, SIGUSR1);
    kill(catcher_pid, SIGUSR2);
  } else if (type == TYPE_SIGQUEUE) {
    union sigval val;
    for (int i = 0; i < signals_num; i++)
      sigqueue(catcher_pid, SIGUSR1, val);
    sigqueue(catcher_pid, SIGUSR2, val);
  } else if (type == TYPE_SIGRT) {
    for (int i = 0; i < signals_num; i++)
      kill(catcher_pid, SIGRTMIN);
    kill(catcher_pid, SIGRTMIN + 1);
  }

  while (1)
    ;
}