#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
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
  // Oczekiwanie na zakończenie procesów użytkowników
  int status;
  while (wait(&status) > 0 || (wait(&status) == -1 && errno == EINTR));
  printf("\nExiting...\n");

  // Usunięcie pamięci współdzielonej
  if (munmap(memory_map, sizeof(memory_map_t)) == -1) perror("munmap");

  exit(exit_status);
}

// Obsługa sygnału SIGINT
void SIGINT_handler(int signum) { clean_exit(EXIT_SUCCESS); }

void generate_random_string(char* buffer, int length) {
  for (int i = 0; i < length; i++) {
    buffer[i] = 'a' + rand() % ('z' - 'a' + 1);
  }
  buffer[length] = '\0';
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("%s <number_of_users>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int number_of_users = atoi(argv[1]);

  // Deskryptor pamięci współdzielonej
  int memory_fd = shm_open(SHARED_MEMORY_DESCRIPTOR, O_RDWR, S_IRUSR | S_IWUSR);
  if (memory_fd < 0) perror("shm_open");

  // Mapowanie pamięci współdzielonej
  memory_map_t* memory_map =
      mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE, MAP_SHARED,
           memory_fd, 0);
  if (memory_map == MAP_FAILED) perror("mmap");

  // Buffor użytkownika
  char user_buffer[MAX_PRINTER_BUFFER_SIZE] = {};

  // Utworzenie procesów użytkowników
  for (int i = 0; i < number_of_users; i++) {
    pid_t user_pid = fork();
    if (user_pid == -1) {
      perror("fork");
      return EXIT_FAILURE;
    } else if (user_pid == 0) {
      // Proces użytkownika
      // Ustawienie ziarna generatora liczb pseudolosowych
      srand(getpid());
      // Generowanie danych
      while (1) {
        // Generowanie danych do wydruku
        generate_random_string(user_buffer, 10);

        // Sprawdzenie, czy jest wolna drukarka
        int printer_index = -1;
        for (int j = 0; j < memory_map->number_of_printers; j++) {
          int val;
          sem_getvalue(&memory_map->printers[j].semaphore, &val);
          if (val > 0) {
            printer_index = j;
            break;
          }
        }

        // Jeśli nie ma wolnej drukarki, wybierz losową
        if (printer_index == -1)
          printer_index = rand() % memory_map->number_of_printers;

        // Czekaj na dostęp do drukarki
        if (sem_wait(&memory_map->printers[printer_index].semaphore) <
            0)
          perror("sem_wait");

        // Skopiuj dane do bufora drukarki
        memcpy(memory_map->printers[printer_index].buffer, user_buffer,
               MAX_PRINTER_BUFFER_SIZE);
        memory_map->printers[printer_index].buffer_size =
            strlen(user_buffer);

        // Rozpocznij drukowanie
        memory_map->printers[printer_index].state = PRINT;

        printf("(User %d -> Printer %d) %s\n", i, printer_index, user_buffer);

        // Oczekuj losowy czas przed wygenerowaniem nowych danych
        sleep(rand() % 3 + 1);
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