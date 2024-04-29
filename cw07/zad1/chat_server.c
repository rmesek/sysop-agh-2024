#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "chat_spec.h"

mqd_t server_queue = {-1};
mqd_t client_queues[MAX_CLIENTS_COUNT];

void shutdown_server() {
  printf("\nServer shutting down\n");

  for (int id = 0; id < MAX_CLIENTS_COUNT; ++id) {
    if (client_queues[id] != -1) mq_close(client_queues[id]);
  }
  mq_close(server_queue);
  mq_unlink(SERVER_QUEUE);

  exit(EXIT_SUCCESS);
}

void SIGINT_handler(int signum) { shutdown_server(); }

mqd_t init_client(int client_id, message_t message) {
  mqd_t client_queue = mq_open(message.text, O_RDWR, S_IRUSR | S_IWUSR, NULL);
  message_t response = {.type = IDENTIFIER, .identifier = client_id};
  int mq_send_return =
      mq_send(client_queue, (char *)&response, sizeof(message_t), 0);
  if (mq_send_return == -1) return -1;

  return client_queue;
}

void broadcast_message(mqd_t *client_queues, message_t message) {
  for (int id = 0; id < MAX_CLIENTS_COUNT; ++id) {
    if (client_queues[id] != -1 && id != message.identifier) {
      int mq_send_return =
          mq_send(client_queues[id], (char *)&message, sizeof(message_t), 0);
      if (mq_send_return == -1) perror("mq_send");
    }
  }
}

int main() {
  // Initialize client queues
  for (int id = 0; id < MAX_CLIENTS_COUNT; ++id) {
    client_queues[id] = -1;
  }

  // Setup SIGINT handler
  struct sigaction action = {.sa_handler = SIGINT_handler};
  if (sigaction(SIGINT, &action, NULL) == -1) {
    perror("sigaction");
    return EXIT_FAILURE;
  }

  // Create server queue
  struct mq_attr attributes = {
      .mq_flags = 0,
      .mq_maxmsg = 10,
      .mq_msgsize = sizeof(message_t),
  };
  mq_unlink(SERVER_QUEUE);
  server_queue =
      mq_open(SERVER_QUEUE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attributes);
  if (server_queue == -1) {
    perror("mq_open server");
    return EXIT_FAILURE;
  }
  message_t received_msg;
  printf("Server running\n");
  while (1) {
    if (mq_receive(server_queue, (char *)&received_msg, sizeof(message_t),
                   NULL) == -1) {
      perror("mq_receive");
      return EXIT_FAILURE;
    }

    switch (received_msg.type) {
      case INIT: {
        // Find first available client slot
        int client_id = 0;
        while (client_id < MAX_CLIENTS_COUNT &&
               client_queues[client_id] != -1) {
          ++client_id;
        }
        if (client_id == MAX_CLIENTS_COUNT) {
          printf("No available client slots\n");
          break;
        }

        // Initialize client
        client_queues[client_id] = init_client(client_id, received_msg);
        if (client_queues[client_id] == -1) {
          perror("init_client");
          return EXIT_FAILURE;
        }
        printf("Client %d connected\n", client_id);
        break;
      }

      case MESSAGE_TEXT: {
        broadcast_message(client_queues, received_msg);
        printf("Client %d: %s", received_msg.identifier, received_msg.text);
        break;
      }

      case CLIENT_CLOSE: {
        mq_close(client_queues[received_msg.identifier]);
        client_queues[received_msg.identifier] = -1;
        printf("Client %d closed connection\n", received_msg.identifier);
        break;
      }

      default: {
        perror("Unknown message type");
        return EXIT_FAILURE;
      }
    }
  }
}