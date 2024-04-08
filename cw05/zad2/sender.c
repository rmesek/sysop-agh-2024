#define _XOPEN_SOURCE 700  // for sigset_t
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void SIGUSR1_handler(int signo) { printf("Confirmation received\n"); }

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("sender <catcher_pid> <mode>\n");
    return EXIT_FAILURE;
  }

  pid_t catcher_pid = atoi(argv[1]);
  int mode = atoi(argv[2]);
  sigset_t mask;

  printf("Sender running with PID: %d\n", getpid());

  signal(SIGUSR1, SIGUSR1_handler);

  // store the mode in the sigval union
  union sigval value;
  value.sival_int = mode;

  printf("Sending mode: %d (Catcher PID: %d)\n", mode, catcher_pid);
  if (sigqueue(catcher_pid, SIGUSR1, value) == -1) {
    perror("sigqueue");
    return EXIT_FAILURE;
  }

  // block all signals
  sigfillset(&mask);

  // unblock SIGUSR1 and SIGINT
  sigdelset(&mask, SIGUSR1);
  sigdelset(&mask, SIGINT);

  printf("Waiting for confirmation from catcher...\n");
  sigsuspend(&mask);
  printf("Exiting.\n");
  return EXIT_SUCCESS;
}