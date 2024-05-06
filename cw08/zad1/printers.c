#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "printers_spec.h"

memory_map_t* memory_map;

void clean_exit(int exit_status) {
  // Oczekiwanie na zakończenie procesów drukarek
  int status;
  while (wait(&status) > 0 || (wait(&status) == -1 && errno == EINTR));
  printf("\nExiting...\n");

  // Usunięcie semaforów
  for (int i = 0; i < memory_map->number_of_printers; ++i)
    if (sem_destroy(&memory_map->printers[i].semaphore) == -1)
      perror("sem_destroy");

  // Usunięcie pamięci współdzielonej
  if (munmap(memory_map, sizeof(memory_map_t)) == -1) perror("munmap");

  // Zamknięcie deskryptora pamięci współdzielonej
  if (shm_unlink(SHARED_MEMORY_DESCRIPTOR) == -1) perror("shm_unlink");

  exit(exit_status);
}

// Obsługa sygnału SIGINT
void SIGINT_handler(int signum) { clean_exit(EXIT_SUCCESS); }

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("%s <number_of_printers>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int number_of_printers = atoi(argv[1]);

  if (number_of_printers > MAX_PRINTERS) {
    fprintf(stderr, "number_of_printers > MAX_PRINTERS = %d\n", MAX_PRINTERS);
    return EXIT_FAILURE;
  }

  // Deskryptor pamięci współdzielonej
  int memory_fd =
      shm_open(SHARED_MEMORY_DESCRIPTOR, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (memory_fd < 0) perror("shm_open");

  // Ustawienie rozmiaru pamięci współdzielonej
  if (ftruncate(memory_fd, sizeof(memory_map_t)) < 0) perror("ftruncate");

  // Mapowanie pamięci współdzielonej
  memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE,
                    MAP_SHARED, memory_fd, 0);
  if (memory_map == MAP_FAILED) perror("mmap");

  // Zerowanie pamięci współdzielonej
  memset(memory_map, 0, sizeof(memory_map_t));

  // Ustawienie liczby drukarek
  memory_map->number_of_printers = number_of_printers;

  // Utworzenie procesów drukarek
  for (int i = 0; i < number_of_printers; i++) {
    // Inicjalizacja semafora
    sem_init(&memory_map->printers[i].semaphore, 1, 1);

    pid_t printer_pid = fork();
    if (printer_pid == -1) {
      perror("fork");
      return EXIT_FAILURE;
    } else if (printer_pid == 0) {
      // Proces drukarki
      while (1) {
        // Oczekiwanie na dane do wydruku
        if (memory_map->printers[i].state == PRINT) {
          // Wydruk danych
          for (int j = 0; j < memory_map->printers[i].buffer_size;
               j++) {
            printf("(Printer %d) ", i);
            printf("%c\n", memory_map->printers[i].buffer[j]);
            // fflush(stdout);
            sleep(1);
          }
          // Zakończenie wydruku
          // printf("\n");

          memory_map->printers[i].state = WAIT;

          // Zwolnienie semafora
          sem_post(&memory_map->printers[i].semaphore);
        }
      }

      exit(EXIT_SUCCESS);
    }
  }

  // Obsługa sygnału SIGINT
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIGINT_handler;
  sigaction(SIGINT, &sa, NULL);
  printf("Press Ctrl+C to exit\n");

  clean_exit(EXIT_SUCCESS);
}