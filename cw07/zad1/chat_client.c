#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "chat_spec.h"

int client_id = -1;
char client_queue_name[CLIENT_QUEUE_NAME_SIZE];
mqd_t client_queue = {-1};
mqd_t server_queue = {-1};

void clean_exit(int exit_code) {
  mq_close(client_queue);
  mq_unlink(client_queue_name);
  exit(exit_code);
}

void shutdown_client() {
  printf("\nClosing client\n");
  message_t message = {.type = CLIENT_CLOSE, .identifier = client_id};
  int mq_send_return =
      mq_send(server_queue, (char *)&message, sizeof(message_t), 0);
  if (mq_send_return == -1) {
    perror("mq_send client close");
    clean_exit(EXIT_FAILURE);
  }
  clean_exit(EXIT_SUCCESS);
}

void SIGINT_handler(int signum) { shutdown_client(); }

void sender() {
  message_t message;
  while (fgets(message.text, MESSAGE_BUFFER_SIZE, stdin) != NULL) {
    message.type = MESSAGE_TEXT;
    message.identifier = client_id;

    int mq_send_return =
        mq_send(server_queue, (char *)&message, sizeof(message_t), 0);
    if (mq_send_return == -1) {
      perror("mq_send client");
    }
  }
  kill(getppid(), SIGINT);
}

void receiver() {
  message_t message;
  while (1) {
    ssize_t mq_receive_return =
        mq_receive(client_queue, (char *)&message, sizeof(message_t), NULL);
    if (mq_receive_return == -1) {
      perror("mq_receive");
    }

    if (message.type == MESSAGE_TEXT) {
      printf("Client %d: %s", message.identifier, message.text);
    } else if (message.type == CLIENT_CLOSE) {
      printf("Client %d has disconnected\n", message.identifier);
    }
  }
}

int main() {
  // Create client queue
  snprintf(client_queue_name, CLIENT_QUEUE_NAME_SIZE, CLIENT_QUEUE, getpid());

  struct mq_attr attributes = {
      .mq_flags = 0,
      .mq_maxmsg = 10,
      .mq_msgsize = sizeof(message_t),
  };
  mq_unlink(client_queue_name);
  client_queue = mq_open(client_queue_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR,
                         &attributes);
  if (client_queue == -1) {
    perror("mq_open client_queue");
    clean_exit(EXIT_FAILURE);
  }

  // Open server queue
  server_queue = mq_open(SERVER_QUEUE, O_RDWR, S_IRUSR | S_IWUSR, NULL);
  if (server_queue == -1) {
    perror("mq_open server_queue");
    clean_exit(EXIT_FAILURE);
  }

  // Send INIT message
  message_t init_message = {.type = INIT, .identifier = -1};
  strcpy(init_message.text, client_queue_name);
  int mq_send_return =
      mq_send(server_queue, (char *)&init_message, sizeof(message_t), 0);
  if (mq_send_return == -1) {
    perror("mq_send init");
    clean_exit(EXIT_FAILURE);
  }

  // Receive IDENTIFIER message
  printf("Waiting for client identifier\n");
  message_t response;
  ssize_t mq_receive_return =
      mq_receive(client_queue, (char *)&response, sizeof(message_t), NULL);
  if (mq_receive_return == -1) {
    perror("mq_receive");
    clean_exit(EXIT_FAILURE);
  }

  client_id = response.identifier;
  printf("Assigned Client ID: %d\n", client_id);

  pid_t pid = fork();
  if (pid == 0) {
    sender();
    clean_exit(EXIT_SUCCESS);
  }
  if (pid == -1) {
    perror("fork");
    clean_exit(EXIT_FAILURE);
  }
  struct sigaction action = {.sa_handler = SIGINT_handler};
  if (sigaction(SIGINT, &action, NULL) == -1) {
    perror("sigaction");
    clean_exit(EXIT_FAILURE);
  }
  receiver();
  clean_exit(EXIT_SUCCESS);
}