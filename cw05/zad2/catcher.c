#define _XOPEN_SOURCE 700  // for sigset_t
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

volatile sig_atomic_t mode = 0;
volatile sig_atomic_t requests = 0;

void SIGUSR1_handler(int signo, siginfo_t *info, void *context) {
  mode = info->si_int;
  requests++;
  printf("Received SIGUSR1 mode: %d (Sender's PID: %d)\n", mode, info->si_pid);

  kill(info->si_pid, SIGUSR1);
}

int main() {
  printf("Catcher running with PID: %d\n", getpid());

  struct sigaction action;
  action.sa_sigaction = SIGUSR1_handler;
  action.sa_flags = SA_SIGINFO;
  sigemptyset(&action.sa_mask);

  if (sigaction(SIGUSR1, &action, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  while (1) {
    pause();
    switch (mode) {
      case 1:
        for (int i = 1; i <= 100; i++) printf("%d ", i);
        printf("\n");
        mode = 0;
        break;
      case 2:
        printf("Requests received: %d\n", requests);
        mode = 0;
        break;
      case 3:
        printf("Received termination request. Exiting.\n");
        exit(EXIT_SUCCESS);
        break;
      default:
        break;
    }
  }

  return EXIT_SUCCESS;
}