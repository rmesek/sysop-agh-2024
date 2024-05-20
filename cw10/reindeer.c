#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define DELIVERIES 4

// Mutex do synchronizacji dostępu do wspólnych zasobów
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Zmienna warunkowa dla Świętego Mikołaja
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
// Zmienna warunkowa dla reniferów
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;

// Liczba reniferów czekających na Mikołaja
int waiting_reindeer = 0;
// Liczba zrealizowanych dostaw
int deliveries = 0;

// Funkcja wątku Świętego Mikołaja
void* santa_thread(void* arg) {
  // Powtarzaj, dopóki nie zrealizujemy wszystkich dostaw
  while (deliveries < DELIVERIES) {
    // Zablokuj mutex
    pthread_mutex_lock(&mutex);
    // Czekaj, dopóki nie wrócą wszystkie renifery
    while (waiting_reindeer < NUM_REINDEER) {
      // Czekaj na sygnał od reniferów
      pthread_cond_wait(&santa_cond, &mutex);
    }

    // Obudzenie i dostarczanie zabawek
    printf("Mikołaj: budzę się\n");
    printf("Mikołaj: dostarczam zabawki\n");
    // Symulacja dostarczania zabawek (2-4 sekundy)
    sleep(rand() % 3 + 2);
    // Zwiększ liczbę dostaw
    deliveries++;
    // Resetuj licznik reniferów
    waiting_reindeer = 0;

    // Powiadomienie reniferów o zakończeniu dostawy
    pthread_cond_broadcast(&reindeer_cond);
    printf("Mikołaj: zasypiam\n");
    // Odblokuj mutex
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

// Funkcja wątku renifera
void* reindeer_thread(void* arg) {
  // ID renifera
  int id = *(int*)arg;
  // Zwolnij pamięć
  free(arg);

  while (1) {
    // Renifer na wakacjach (5-10 sekund)
    sleep(rand() % 6 + 5);
    // Zablokuj mutex
    pthread_mutex_lock(&mutex);
    // Zwiększ licznik czekających reniferów
    waiting_reindeer++;
    printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n",
           waiting_reindeer, id);

    // Jeśli to dziewiąty renifer
    if (waiting_reindeer == NUM_REINDEER) {
      printf("Renifer: wybudzam Mikołaja, ID: %d\n", id);
      // Wybudź Mikołaja
      pthread_cond_signal(&santa_cond);
    }
    // Czekaj na zakończenie dostawy zabawek
    pthread_cond_wait(&reindeer_cond, &mutex);
    // Odblokuj mutex
    pthread_mutex_unlock(&mutex);

    // Zakończ, jeśli wszystkie dostawy zrealizowane
    if (deliveries >= DELIVERIES) break;
  }

  return NULL;
}

int main() {
  srand(time(NULL));

  // Wątek Świętego Mikołaja
  pthread_t santa;
  // Wątki reniferów

  pthread_t reindeer[NUM_REINDEER];
  // Utwórz wątek Świętego Mikołaja
  pthread_create(&santa, NULL, santa_thread, NULL);

  for (int i = 0; i < NUM_REINDEER; i++) {
    // Dynamicznie alokuj pamięć dla ID renifera
    int* id = malloc(sizeof(int));
    // Ustaw ID renifera
    *id = i + 1;
    // Utwórz wątek renifera
    pthread_create(&reindeer[i], NULL, reindeer_thread, id);
  }

  // Poczekaj na zakończenie wątku Świętego Mikołaja
  pthread_join(santa, NULL);

  // Poczekaj na zakończenie wątków reniferów
  for (int i = 0; i < NUM_REINDEER; i++) pthread_join(reindeer[i], NULL);

  // Zniszcz mutex i zmienne warunkowe
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&santa_cond);
  pthread_cond_destroy(&reindeer_cond);

  return EXIT_SUCCESS;
}
