#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
  int num_processes;

  if (argc != 2) {
    printf("child <number_of_processes>\n");
    return EXIT_FAILURE;
  }

  num_processes = atoi(argv[1]);

  for (int i = 0; i < num_processes; ++i) {
    pid_t pid = fork();

    if (pid == -1) {
      perror("Forking error");
      return EXIT_FAILURE;
    } else if (pid == 0) {  // child process
      printf("parent pid = %d, child pid = %d\n", getppid(), getpid());
      return EXIT_SUCCESS;
    }
  }

  // parent process
  for (int i = 0; i < num_processes; ++i) {
    wait(NULL);
  }

  printf("argv[1] = %s\n", argv[1]);

  return EXIT_SUCCESS;
}