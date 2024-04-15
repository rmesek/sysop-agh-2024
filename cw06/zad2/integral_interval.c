#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "integral_pipe.h"

int main() {
  integral_args_t args;
  int read_fd, write_fd;
  double area;

  if ((write_fd = open(WRITE_PIPE_NAME, O_WRONLY)) == -1) {
    perror("open");
    return EXIT_FAILURE;
  }
  if ((read_fd = open(READ_PIPE_NAME, O_RDONLY)) == -1) {
    perror("open");
    return EXIT_FAILURE;
  }

  while (1) {
    printf("Input <a> <b> <intervals_number>: ");
    if (scanf("%lf %lf %lf", &args.a, &args.b, &args.intervals_number) != 3) {
      fprintf(stderr, "Invalid input\n");
      return EXIT_FAILURE;
    }

    if (write(write_fd, &args, sizeof(args)) == -1) {
      perror("write");
      return EXIT_FAILURE;
    }

    if (read(read_fd, &area, sizeof(area)) == -1) {
      perror("read");
      return EXIT_FAILURE;
    }

    printf("Area: %lf\n", area);
  }

  close(read_fd);
  close(write_fd);

  return EXIT_SUCCESS;
}