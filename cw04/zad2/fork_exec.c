#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int global = 0;
int main(int argc, char** argv) {
  int local = 0;
  int status;
  pid_t pid;

  if (argc != 2) {
    printf("fork_exec <directory>\n");
    return EXIT_FAILURE;
  }

  printf("Program: %s\n", argv[0]);

  pid = fork();

  if (pid == -1) {
    perror("Forking error");
    return EXIT_FAILURE;
  } else if (pid == 0) {  // child process
    printf("child process\n");
    global++;
    local++;
    printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
    printf("child's local = %d, child's global = %d\n", local, global);
    execl("/bin/ls", "ls", argv[1], NULL);
    perror("execl() error");
    return EXIT_FAILURE;
  } else {  // parent process
    printf("parent process\n");
    printf("parent pid = %d, child pid = %d\n", getpid(), pid);
    waitpid(pid, &status, 0);
    printf("Child exit code: %d\n", WEXITSTATUS(status));
    printf("Parent's local = %d, parent's global = %d\n", local, global);
    return WEXITSTATUS(status);
  }

  return EXIT_SUCCESS;
}