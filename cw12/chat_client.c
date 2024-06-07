#include "chat_spec.h"

char *client_name;

// Funkcja obsługująca wiadomości otrzymywane od serwera
void handle_server_message(int server_socket) {
  Message msg;
  while (recv(server_socket, &msg, sizeof(Message), 0) > 0) {
    switch (msg.type) {
      // Obsługa wiadomości typu LIST
      case LIST:
        printf("%s\n", msg.message);
        break;
      // Obsługa wiadomości typu TO_ALL i TO_ONE
      case TO_ALL:
      case TO_ONE: {
        struct tm *tm_info = localtime(&msg.timestamp);
        char time_str[26];
        strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        printf("[%s] %s: %s\n", time_str, msg.sender, msg.message);
        break;
      }
      // (ping serwera)
      case ALIVE:
        // Odpowiedź na ping
        msg.type = ALIVE;
        strncpy(msg.sender, client_name, MAX_NAME_LEN);
        send(server_socket, &msg, sizeof(Message), 0);
        break;
      default:
        break;
    }
  }
  printf("Server disconnected\n");
  exit(EXIT_SUCCESS);
}

// Funkcja wątku do czytania wiadomości od serwera
void *read_server_messages(void *arg) {
  int server_socket = *(int *)arg;
  handle_server_message(server_socket);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <client_name> <server_ip> <server_port>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  client_name = argv[1];
  char *server_ip = argv[2];
  int server_port = atoi(argv[3]);

  if (strlen(client_name) >= MAX_NAME_LEN) {
    fprintf(stderr, "Client name is too long. Max length is %d\n",
            MAX_NAME_LEN - 1);
    exit(EXIT_FAILURE);
  }

  int server_socket;
  struct sockaddr_in server_address;

  // Utworzenie gniazda klienta
  server_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_socket < 0) error("ERROR opening socket");

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(server_ip);
  server_address.sin_port = htons(server_port);

  // Połączenie z serwerem (send/recv instead of sendto/recvfrom)
  if (connect(server_socket, (struct sockaddr *)&server_address,
              sizeof(server_address)) < 0)
    error("ERROR connecting");

  // Próba uzyskania dostępu do serwera
  Message msg;
  msg.type = ALIVE;
  strncpy(msg.sender, client_name, MAX_NAME_LEN);
  send(server_socket, &msg, sizeof(Message), 0);

  // Odbiór odpowiedzi od serwera
  recv(server_socket, &msg, sizeof(Message), 0);
  if (msg.type != ALIVE) error("Server is not available or refused connection");
  printf("Connected to server\n");

  // Utworzenie wątku do czytania wiadomości od serwera
  pthread_t server_thread;
  pthread_create(&server_thread, NULL, read_server_messages,
                 (void *)&server_socket);

  char buffer[MAX_MSG_LEN + MAX_NAME_LEN + 10];
  while (1) {
    // Odczytanie wejścia z konsoli
    memset(buffer, 0, sizeof(buffer));
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      break;
    }

    if (strncmp(buffer, "LIST", 4) == 0) {
      // Wysłanie żądania LIST do serwera
      Message msg;
      msg.type = LIST;
      strncpy(msg.sender, client_name, MAX_NAME_LEN);
      send(server_socket, &msg, sizeof(Message), 0);
    } else if (strncmp(buffer, "2ALL ", 5) == 0) {
      // Wysłanie wiadomości do wszystkich klientów
      char *message = strtok(buffer + 5, "\n");
      Message msg;
      msg.type = TO_ALL;
      strncpy(msg.sender, client_name, MAX_NAME_LEN);
      strncpy(msg.message, message, MAX_MSG_LEN);
      send(server_socket, &msg, sizeof(Message), 0);
    } else if (strncmp(buffer, "2ONE ", 5) == 0) {
      // Wysłanie wiadomości do konkretnego klienta
      char *receiver = strtok(buffer + 5, " ");
      char *message = strtok(NULL, "\n");
      if (receiver && message) {
        Message msg;
        msg.type = TO_ONE;
        strncpy(msg.sender, client_name, MAX_NAME_LEN);
        strncpy(msg.receiver, receiver, MAX_NAME_LEN);
        strncpy(msg.message, message, MAX_MSG_LEN);
        send(server_socket, &msg, sizeof(Message), 0);
      }
    } else if (strncmp(buffer, "STOP", 4) == 0) {
      // Wysłanie żądania STOP do serwera i zakończenie pracy
      Message msg;
      msg.type = STOP;
      strncpy(msg.sender, client_name, MAX_NAME_LEN);
      send(server_socket, &msg, sizeof(Message), 0);
      break;
    } else {
      printf("Unknown command!\n");
    }
  }

  // Zamknięcie gniazda klienta
  close(server_socket);
  return EXIT_SUCCESS;
}
