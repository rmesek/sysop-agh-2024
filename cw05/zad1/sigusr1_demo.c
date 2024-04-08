#define _XOPEN_SOURCE 700  // for sigset_t
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void signal_handler(int signo) {
  printf("Handling SIGUSR1 with signal_handler!\n");
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("sigusr1_demo <none/ignore/handler/mask>\n");
    return EXIT_FAILURE;
  }
  sigset_t mask;

  if (strcmp(argv[1], "none") == 0) {
    // default reaction
  } else if (strcmp(argv[1], "ignore") == 0) {
    // ignore signal
    signal(SIGUSR1, SIG_IGN);
  } else if (strcmp(argv[1], "handler") == 0) {
    // set handler
    signal(SIGUSR1, signal_handler);
  } else if (strcmp(argv[1], "mask") == 0) {
    // mask signal
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);
  } else {
    fprintf(stderr, "Wrong setting!\n");
    return EXIT_FAILURE;
  }

  raise(SIGUSR1);
  // while (1) sleep(1);

  if (sigismember(&mask, SIGUSR1)) {
    printf("SIGUSR1 is masked.\n");
  }

  // sigpending wypisać

  return EXIT_SUCCESS;
}
