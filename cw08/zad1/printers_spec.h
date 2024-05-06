#ifndef PRINTERS_SPEC_H
#define PRINTERS_SPEC_H

#define SHARED_MEMORY_DESCRIPTOR "printers_shared_memory"
#include <semaphore.h>

#define MAX_PRINTERS 32
#define MAX_PRINTER_BUFFER_SIZE 256
#define MAX_SEMAPHORE_NAME 40

typedef enum { WAIT = 0, PRINT = 1 } printer_state_t;

typedef struct {
  sem_t semaphore;
  char buffer[MAX_PRINTER_BUFFER_SIZE];
  size_t buffer_size;
  printer_state_t state;
} printer_t;

typedef struct {
  printer_t printers[MAX_PRINTERS];
  int number_of_printers;
} memory_map_t;

#endif  // PRINTERS_SPEC_H