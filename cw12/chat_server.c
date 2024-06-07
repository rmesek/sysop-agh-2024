#include "chat_spec.h"

// Tablica przechowująca informacje o klientach
Client clients[MAX_CLIENTS];
// Mutex do synchronizacji dostępu do tablicy klientów
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Funkcja porównująca dwie struktury sockaddr_in
int compare_sockaddr_in(struct sockaddr_in *addr1, struct sockaddr_in *addr2) {
  // Compare IP addresses
  if (addr1->sin_addr.s_addr != addr2->sin_addr.s_addr) {
    return 0;  // Addresses are not equal
  }
  // Compare port numbers
  if (addr1->sin_port != addr2->sin_port) {
    return 0;  // Addresses are not equal
  }
  return 1;  // Addresses are equal
}

// Funkcja dodająca nowego klienta do tablicy klientów
void add_client(Message msg, int server_socket,
                struct sockaddr_in client_address) {
  pthread_mutex_lock(&clients_mutex);
  printf("Adding new client\n");
  char name[MAX_NAME_LEN];
  strncpy(name, msg.sender, MAX_NAME_LEN);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!clients[i].active) {
      // Dodanie nowego klienta do tablicy
      strncpy(clients[i].name, name, MAX_NAME_LEN);
      clients[i].address = client_address;
      clients[i].active = 1;
      clients[i].last_active = time(NULL);
      sendto(server_socket, &msg, sizeof(Message), 0,
             (struct sockaddr *)&client_address, sizeof(client_address));
      printf("Client connected: %s [%s:%d] \n", name,
             inet_ntoa(client_address.sin_addr),
             ntohs(client_address.sin_port));
      pthread_mutex_unlock(&clients_mutex);
      return;
    }
  }
  // Nie udało się dodać nowego klienta
  printf("Client refused: %s [%s:%d] (limit reached!)\n", name,
         inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja usuwająca klienta z tablicy klientów
void remove_client(int server_socket, struct sockaddr_in client_address) {
  pthread_mutex_lock(&clients_mutex);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (compare_sockaddr_in(&clients[i].address, &client_address)) {
      printf("Client disconnected: %s [%s:%d]\n", clients[i].name,
             inet_ntoa(client_address.sin_addr),
             ntohs(client_address.sin_port));
      clients[i].active = 0;
      // Poinformowanie klienta o rozłączeniu
      Message msg;
      msg.type = STOP;
      strncpy(msg.sender, "SERVER", MAX_NAME_LEN);
      sendto(server_socket, &msg, sizeof(Message), 0,
             (struct sockaddr *)&client_address, sizeof(client_address));
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wysyłająca wiadomość do wszystkich klientów
void send_message_to_all(char *sender, char *message, int server_socket,
                         struct sockaddr_in client_address) {
  pthread_mutex_lock(&clients_mutex);
  Message msg;
  msg.type = TO_ALL;
  strncpy(msg.sender, sender, MAX_NAME_LEN);
  strncpy(msg.message, message, MAX_MSG_LEN);
  msg.timestamp = time(NULL);
  printf("Broadcast message from %s [%s:%d]: %s\n", sender,
         inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
         message);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active) {
      // Wysłanie wiadomości do aktywnych klientów
      sendto(server_socket, &msg, sizeof(Message), 0,
             (struct sockaddr *)&clients[i].address,
             sizeof(clients[i].address));
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wysyłająca wiadomość do konkretnego klienta
void send_message_to_one(char *sender, char *receiver, char *message,
                         int server_socket, struct sockaddr_in client_address) {
  pthread_mutex_lock(&clients_mutex);
  Message msg;
  msg.type = TO_ONE;
  strncpy(msg.sender, sender, MAX_NAME_LEN);
  strncpy(msg.receiver, receiver, MAX_NAME_LEN);
  strncpy(msg.message, message, MAX_MSG_LEN);
  msg.timestamp = time(NULL);
  printf("Message from %s [%s:%d] to %s: %s\n", sender,
         inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port),
         receiver, message);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active && strcmp(clients[i].name, receiver) == 0) {
      // Wysyłanie wiadomości do wszystkich klientów o danej nazwie
      sendto(server_socket, &msg, sizeof(Message), 0,
             (struct sockaddr *)&clients[i].address,
             sizeof(clients[i].address));
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
      // TODO: Dodanie adresu IP klienta
      printf("%s [%s:%d]; ", clients[i].name,
             inet_ntoa(clients[i].address.sin_addr),
             ntohs(clients[i].address.sin_port));
    }
  }
  printf("\n");
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja wysyłająca listę aktywnych klientów do konkretnego klienta
void list_clients(int server_socket, struct sockaddr_in client_address) {
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
  sendto(server_socket, &msg, sizeof(Message), 0,
         (struct sockaddr *)&client_address, sizeof(client_address));
  printf("Sent list of active clients\n");
  pthread_mutex_unlock(&clients_mutex);
}

// Funkcja obsługująca komunikację z klientem poprzez ip i port
void handle_alive(Message msg, int server_socket,
                  struct sockaddr_in client_address) {
  pthread_mutex_lock(&clients_mutex);
  int client_id = -1;
  // Znalezienie klienta w tablicy
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (compare_sockaddr_in(&clients[i].address, &client_address)) {
      client_id = i;
      break;
    }
  }
  // Dodanie nowego klienta, jeśli nie istnieje
  if (client_id == -1) {
    // Próba dodania nowego klienta
    pthread_mutex_unlock(&clients_mutex);
    add_client(msg, server_socket, client_address);
    pthread_mutex_lock(&clients_mutex);
  } else {
    // Uaktualnienie znacznika czasu ostatniej aktywności klienta
    clients[client_id].last_active = time(NULL);
  }
  pthread_mutex_unlock(&clients_mutex);
}

int is_active_client(struct sockaddr_in client_address) {
  pthread_mutex_lock(&clients_mutex);
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (clients[i].active &&
        compare_sockaddr_in(&clients[i].address, &client_address)) {
      pthread_mutex_unlock(&clients_mutex);
      return 1;
    }
  }
  pthread_mutex_unlock(&clients_mutex);
  return 0;
}

// Funkcja obsługująca komunikację z klientem
void *handle_client(void *arg) {
  int server_socket = *(int *)arg;
  struct sockaddr_in client_address;
  socklen_t client_len = sizeof(client_address);

  Message msg;
  while (recvfrom(server_socket, &msg, sizeof(Message), 0,
                  (struct sockaddr *)&client_address, &client_len) > 0) {
    switch (msg.type) {
      case LIST:
        printf("Received LIST request from %s [%s:%d]\n", msg.sender,
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));
        if (!is_active_client(client_address)) {
          printf("Refused (not active!)\n");
          break;
        }
        list_clients(server_socket, client_address);
        break;
      case TO_ALL:
        printf("Received message from %s [%s:%d]: %s\n", msg.sender,
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port), msg.message);
        if (!is_active_client(client_address)) {
          printf("Refused (not active!)\n");
          break;
        }
        send_message_to_all(msg.sender, msg.message, server_socket,
                            client_address);
        break;
      case TO_ONE:
        printf("Received message from %s [%s:%d] to %s: %s\n", msg.sender,
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port), msg.receiver, msg.message);
        if (!is_active_client(client_address)) {
          printf("Refused (not active!)\n");
          break;
        }
        send_message_to_one(msg.sender, msg.receiver, msg.message,
                            server_socket, client_address);
        break;
      case STOP:
        printf("Received STOP from %s [%s:%d]\n", msg.sender,
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));
        if (!is_active_client(client_address)) {
          printf("Refused (not active!)\n");
          break;
        }
        remove_client(server_socket, client_address);
        break;
      case ALIVE:
        printf("Received ALIVE from %s [%s:%d]\n", msg.sender,
               inet_ntoa(client_address.sin_addr),
               ntohs(client_address.sin_port));
        handle_alive(msg, server_socket, client_address);
        break;
      default:
        break;
    }
  }

  // Usunięcie klienta w przypadku zakończenia połączenia
  return NULL;
}

// Funkcja pingująca klientów, aby sprawdzić ich aktywność
void *ping_clients(void *arg) {
  int server_socket = *(int *)arg;
  while (1) {
    // Oczekiwanie przez ustalony interwał czasu
    sleep(SERVER_PING_INTERVAL);
    pthread_mutex_lock(&clients_mutex);
    // Pingowanie aktywnych klientów
    printf("Pinging clients\n");
    Message msg;
    msg.type = ALIVE;
    strncpy(msg.sender, "SERVER", MAX_NAME_LEN);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
      if (clients[i].active) {
        sendto(server_socket, &msg, sizeof(Message), 0,
               (struct sockaddr *)&clients[i].address,
               sizeof(clients[i].address));
      }
    }
    // Usuwanie nieaktywnych klientów
    time_t current_time = time(NULL);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
      if (clients[i].active && difftime(current_time, clients[i].last_active) >
                                   2 * SERVER_PING_INTERVAL) {
        printf("Client timeout: %s [%s:%d]\n", clients[i].name,
               inet_ntoa(clients[i].address.sin_addr),
               ntohs(clients[i].address.sin_port));
        clients[i].active = 0;
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

  int server_socket, port;
  struct sockaddr_in server_address;

  // Utworzenie gniazda serwera
  server_socket = socket(AF_INET, SOCK_DGRAM, 0);
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

  // Utworzenie wątków do pingowania klientów i obsługi wejścia z konsoli
  pthread_t ping_thread, console_thread;
  pthread_create(&ping_thread, NULL, ping_clients, (void *)&server_socket);
  pthread_create(&console_thread, NULL, console_input, NULL);

  printf("Server listening on port %d\n", port);

  // Utworzenie wątku do nasłuchiwania klientów
  pthread_t client_thread;
  pthread_create(&client_thread, NULL, handle_client, (void *)&server_socket);
  pthread_join(client_thread, NULL);

  // Zamknięcie gniazda serwera
  close(server_socket);
  return EXIT_SUCCESS;
}
