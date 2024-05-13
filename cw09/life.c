#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE
#include <locale.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "grid.h"

#define THREADS 8

// Obsługa sygnału wątków (unpause)
void SIGUSR1_handler(int signo) {
  // Do nothing
  return;
}

typedef struct {
  // Zakres komórek dla wątku [start_index, end_index)
  int start_index;
  int end_index;
  char** background;
  char** foreground;
} thread_args_t;

void* thread_function(void* arg) {
  thread_args_t* args = (thread_args_t*)arg;

  while (true) {
    pause();  // Czekaj na sygnał z głównego wątku
    for (int i = args->start_index; i < args->end_index; ++i) {
      int row = i / GRID_WIDTH;
      int col = i % GRID_WIDTH;

      (*args->background)[i] = is_alive(row, col, *args->foreground);
    }
  }
}

int main() {
  // Ustaw obsługę sygnału SIGUSR1
  struct sigaction sa;
  sa.sa_handler = SIGUSR1_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);

  srand(time(NULL));
  setlocale(LC_CTYPE, "");
  initscr();  // Start curses mode

  char* foreground = create_grid();
  char* background = create_grid();
  char* tmp;

  pthread_t threads[THREADS];
  thread_args_t args[THREADS];

  int cells_per_thread = GRID_HEIGHT * GRID_WIDTH / THREADS;
  for (int i = 0; i < THREADS; ++i) {
    args[i].start_index = i * cells_per_thread;
    args[i].end_index = (i + 1) * cells_per_thread;
    // Jeśli to ostatni wątek, to przypisz pozostałe komórki
    if (i == THREADS - 1) args[i].end_index = GRID_HEIGHT * GRID_WIDTH;

    args[i].foreground = &foreground;
    args[i].background = &background;

    // Utwórz wątek z jego parametrami
    pthread_create(&threads[i], NULL, thread_function, &args[i]);
  }

  init_grid(foreground);

  while (true) {
    draw_grid(foreground);

    // Odpauzuj wątki
    for (int i = 0; i < THREADS; ++i) pthread_kill(threads[i], SIGUSR1);

    // Dać czas wątkom na wykonanie
    usleep(500 * 1000);

    // Step simulation
    // update_grid(foreground, background);
    // background[rand() % (GRID_HEIGHT * GRID_WIDTH)] = 1;  // TODO: Delete

    tmp = foreground;
    foreground = background;
    background = tmp;
  }

  endwin();  // End curses mode
  destroy_grid(background);
  destroy_grid(foreground);

  return 0;
}