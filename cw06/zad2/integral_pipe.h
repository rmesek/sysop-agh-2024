#ifndef INTEGRAL_PIPE_H
#define INTEGRAL_PIPE_H

#define READ_PIPE_NAME "read_pipe.fifo"
#define WRITE_PIPE_NAME "write_pipe.fifo"

typedef struct {
  double a;
  double b;
  double intervals_number;
} integral_args_t;

#endif // INTEGRAL_PIPE_H