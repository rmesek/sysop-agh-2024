#ifndef CHAT_SPEC_H
#define CHAT_SPEC_H

#define MESSAGE_BUFFER_SIZE 2048
#define MAX_CLIENTS_COUNT 3
#define SERVER_QUEUE "/chat_server_queue"
#define CLIENT_QUEUE "/chat_client_queue_%d"
#define CLIENT_QUEUE_NAME_SIZE 50

typedef enum {
  INIT,          // Client sends this message to server to initialize connection
  IDENTIFIER,    // Server sends this message to client with client identifier
  MESSAGE_TEXT,  // Server sends this message to client with text message
  CLIENT_CLOSE   // Client sends this message to server to close connection
} message_type_t;

typedef struct {
  message_type_t type;

  int identifier;
  char text[MESSAGE_BUFFER_SIZE];
} message_t;

#endif  // CHAT_SPEC_H