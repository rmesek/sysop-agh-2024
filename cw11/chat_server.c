#include "chat_spec.h"

// Tablica przechowująca informacje o klientach
Client clients[MAX_CLIENTS];
// Mutex do synchronizacji dostępu do tablicy klientów
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Funkcja dodająca nowego klienta do tablicy klientów
void add_client(int client_socket, struct sockaddr_in client_address,
                char *name) {
  pthread_mutex_lock(&clients_mutex);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!clients[i].active) {
      clients[i].socket = client_socket;
      clients[i].address = client_address;
      strncpy(clients[i].name, name, MAX_NAME_LEN);
      clients[i].active = 1;
      printf("Client connected: %s (socket %d)\n", name, client_socket);
      break;
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja usuwająca klienta z tablicy klientów
void remove_client(int client_socket) {
  pthread_mutex_lock(&clients_mutex);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].socket == client_socket) {
      printf("Client disconnected: %s (socket %d)\n", clients[i].name,
             client_socket);
      clients[i].active = 0;
      // Zamknięcie gniazda
      close(clients[i].socket);
      // break;
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wysyłająca wiadomość do wszystkich klientów
void send_message_to_all(char *sender, char *message) {
  pthread_mutex_lock(&clients_mutex);
  Message msg;
  msg.type = TO_ALL;
  strncpy(msg.sender, sender, MAX_NAME_LEN);
  strncpy(msg.message, message, MAX_MSG_LEN);
  msg.timestamp = time(NULL);
  printf("Broadcast message from %s: %s\n", sender, message);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active) {
      // Wysłanie wiadomości do aktywnych klientów
      write(clients[i].socket, &msg, sizeof(Message));
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wysyłająca wiadomość do konkretnego klienta
void send_message_to_one(char *sender, char *receiver, char *message) {
  pthread_mutex_lock(&clients_mutex);
  Message msg;
  msg.type = TO_ONE;
  strncpy(msg.sender, sender, MAX_NAME_LEN);
  strncpy(msg.receiver, receiver, MAX_NAME_LEN);
  strncpy(msg.message, message, MAX_MSG_LEN);
  msg.timestamp = time(NULL);
  printf("Message from %s to %s: %s\n", sender, receiver, message);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active && strcmp(clients[i].name, receiver) == 0) {
      // Wysłanie wiadomości do konkretnego klienta
      write(clients[i].socket, &msg, sizeof(Message));
      // Wysyłanie wiadomości do wszystkich klientów o danej nazwie
      // break;
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wyświetlająca listę aktywnych klientów w konsoli serwera
void list_clients_console() {
  pthread_mutex_lock(&clients_mutex);
  printf("Active clients: ");
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active) {
      printf("%s ", clients[i].name);
    }
  }
  printf("\n");
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wysyłająca listę aktywnych klientów do konkretnego klienta
void list_clients(int client_socket) {
  pthread_mutex_lock(&clients_mutex);
  Message msg;
  msg.type = LIST;
  strncpy(msg.sender, "SERVER", MAX_NAME_LEN);
  char list[MAX_MSG_LEN] = "Active clients: ";
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active) {
      strcat(list, clients[i].name);
      strcat(list, " ");
    }
  }
  strncpy(msg.message, list, MAX_MSG_LEN);
  // Wysłanie listy aktywnych klientów
  write(client_socket, &msg, sizeof(Message));
  printf("Sent list of active clients to socket %d\n", client_socket);
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja obsługująca komunikację z klientem
void *handle_client(void *arg) {
  int client_socket = *(int *)arg;
  char client_name[MAX_NAME_LEN];
  // Odczytanie nazwy klienta
  read(client_socket, client_name, MAX_NAME_LEN);
  // Dodanie klienta do listy
  add_client(client_socket, *((struct sockaddr_in *)arg), client_name);
  free(arg);

  Message msg;
  while (read(client_socket, &msg, sizeof(Message)) > 0) {
    switch (msg.type) {
      case LIST:
        list_clients(client_socket);
        break;
      case TO_ALL:
        send_message_to_all(msg.sender, msg.message);
        break;
      case TO_ONE:
        send_message_to_one(msg.sender, msg.receiver, msg.message);
        break;
      case STOP:
        remove_client(client_socket);
        pthread_exit(NULL);
        break;
      default:
        break;
    }
  }

  // Usunięcie klienta w przypadku zakończenia połączenia
  remove_client(client_socket);
  pthread_exit(NULL);
}

// Funkcja pingująca klientów, aby sprawdzić ich aktywność
void *ping_clients() {
  while (1) {
    // Oczekiwanie przez ustalony interwał czasu
    sleep(SERVER_PING_INTERVAL);
    pthread_mutex_lock(&clients_mutex);
    Message msg;
    msg.type = ALIVE;
    strncpy(msg.sender, "SERVER", MAX_NAME_LEN);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
      if (clients[i].active) {
        // Wysłanie ping do klienta
        if (write(clients[i].socket, &msg, sizeof(Message)) <= 0) {
          printf("Client %s (socket %d) did not respond to ping, removing\n",
                 clients[i].name, clients[i].socket);
          // Usunięcie nieaktywnego klienta
          remove_client(clients[i].socket);
        }
      }
    }
    pthread_mutex_unlock(&clients_mutex);
  }
  return NULL;
}

// Funkcja obsługująca wejście z konsoli serwera
void *console_input() {
  char buffer[128];
  while (1) {
    // Odczytanie wejścia z konsoli
    fgets(buffer, sizeof(buffer), stdin);
    if (strncmp(buffer, "LIST", 4) == 0) {
      list_clients_console();
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int server_socket, client_socket, port;
  struct sockaddr_in server_address, client_address;
  socklen_t client_len = sizeof(client_address);

  // Utworzenie gniazda serwera
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) error("ERROR opening socket");

  // Wyzerowanie struktury adresu serwera
  memset(&server_address, 0, sizeof(server_address));
  port = atoi(argv[1]);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  // Ustaw numer portu serwera, konwertując z kolejności bajtów hosta na
  // kolejność bajtów sieci
  server_address.sin_port = htons(port);

  // Powiązanie gniazda z adresem serwera
  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) < 0)
    error("ERROR on binding");

  // Oczekiwanie na połączenia od klientów
  listen(server_socket, 5);

  // Utworzenie wątków do pingowania klientów i obsługi wejścia z konsoli
  pthread_t ping_thread, console_thread;
  pthread_create(&ping_thread, NULL, ping_clients, NULL);
  pthread_create(&console_thread, NULL, console_input, NULL);

  printf("Server listening on port %d\n", port);
  // Pętla oczekująca na połączenia od klientów
  while (
      (client_socket = accept(server_socket, (struct sockaddr *)&client_address,
                              &client_len))) {
    int *new_sock = malloc(sizeof(int));
    *new_sock = client_socket;
    pthread_t client_thread;
    // Utworzenie wątku do obsługi klienta (`new_sock` przez referencję)
    pthread_create(&client_thread, NULL, handle_client, (void *)new_sock);
  }
  // Zamknięcie gniazda serwera
  close(server_socket);
  return EXIT_SUCCESS;
}
