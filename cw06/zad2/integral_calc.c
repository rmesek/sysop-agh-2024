#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "integral_pipe.h"

double f(double x) { return 4 / (x * x + 1); }

double integrate(double (*f)(double), integral_args_t* args) {
  double a = args->a;
  double b = args->b;
  unsigned long intervals_number = args->intervals_number;

  double interval_width = (b - a) / intervals_number;

  double area = 0;
  for (double x = a; x < b; x += interval_width) {
    area += f(x) * interval_width;
  }
  return area;
}

void unlink_pipes() {
  unlink(WRITE_PIPE_NAME);
  unlink(READ_PIPE_NAME);
}

int main() {
  int read_fd, write_fd;
  integral_args_t args;
  double area;

  unlink_pipes();
  if (mkfifo(WRITE_PIPE_NAME, S_IRWXU) == -1) {  // IN_PIPE_NAME
    perror("mkfifo");
    unlink_pipes();
    return EXIT_FAILURE;
  }

  if (mkfifo(READ_PIPE_NAME, S_IRWXU) == -1) {
    perror("mkfifo");
    unlink_pipes();
    return EXIT_FAILURE;
  }

  if ((write_fd = open(WRITE_PIPE_NAME, O_RDONLY)) == -1) {
    perror("open");
    unlink_pipes();
    return EXIT_FAILURE;
  }

  if ((read_fd = open(READ_PIPE_NAME, O_WRONLY)) == -1) {
    perror("open");
    unlink_pipes();
    return EXIT_FAILURE;
  }

  while (1) {
    if (read(write_fd, &args, sizeof(args)) == -1) {
      perror("read");
      unlink_pipes();
      return EXIT_FAILURE;
    }

    area = integrate(f, &args);

    if (write(read_fd, &area, sizeof(area)) == -1) {
      perror("write");
      unlink_pipes();
      return EXIT_FAILURE;
    }
  }

  close(write_fd);
  close(read_fd);
  unlink_pipes();

  return EXIT_SUCCESS;
}