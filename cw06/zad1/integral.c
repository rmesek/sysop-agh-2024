#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

double f(double x) { return 4 / (x * x + 1); }
double a = 0, b = 1;

double integrate(double (*f)(double), double a, double b,
                 double interval_width) {
  if (interval_width > b - a) {
    interval_width = b - a;
  }

  double area = 0;
  for (double x = a; x < b; x += interval_width) {
    area += f(x) * interval_width;
  }
  return area;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("%s <interval_width> <n_processes>\n", argv[0]);
    return EXIT_FAILURE;
  }

  double interval_width = atof(argv[1]);
  unsigned long n_processes = atoi(argv[2]);
  int pipes[n_processes][2];
  double part_width = (b - a) / n_processes;

  if (part_width < interval_width) {
    printf("WARNING! Using interval_width: %lf (Too much processes)\n",
           part_width);
  }

  for (unsigned long i = 0; i < n_processes; ++i) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
      perror("fork");
      return EXIT_FAILURE;
    } else if (pid == 0) {
      // child
      close(pipes[i][0]);  // close read end
      double local_area = integrate(f, a + i * part_width,
                                    a + (i + 1) * part_width, interval_width);
      if (write(pipes[i][1], &local_area, sizeof(local_area)) == -1) {
        perror("write");
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }

    close(pipes[i][1]);  // close write end in parent
  }

  double total_area = 0;
  for (unsigned long i = 0; i < n_processes; ++i) {
    double local_area;
    if (read(pipes[i][0], &local_area, sizeof(local_area)) == -1) {
      perror("read");
      return EXIT_FAILURE;
    }
    total_area += local_area;
  }

  printf("Total area: %.20lf\n", total_area);

  return EXIT_SUCCESS;
}