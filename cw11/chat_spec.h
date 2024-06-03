#ifndef CHAT_SPEC_H
#define CHAT_SPEC_H

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Maksymalna liczba klientów, którzy mogą jednocześnie uczestniczyć w czacie
#define MAX_CLIENTS 10

// Maksymalna długość nazwy klienta (identyfikatora)
#define MAX_NAME_LEN 32

// Maksymalna długość wiadomości, którą można wysłać
#define MAX_MSG_LEN 256

// Interwał czasowy, co jaki serwer pingować klientów
#define SERVER_PING_INTERVAL 10  // seconds

typedef struct {
  // deskryptor gniazda klienta
  int socket;
  // nazwa (identyfikator) klienta
  char name[MAX_NAME_LEN];
  // adres IP klienta
  struct sockaddr_in address;
  // flaga aktywności klienta (0 - nieaktywny, 1 - aktywny)
  int active;
} Client;

typedef enum {
  // Pobranie listy aktywnych klientów
  LIST,
  // Wiadomość do wszystkich klientów
  TO_ALL,
  // Wiadomość do konkretnego klienta
  TO_ONE,
  // Zgłoszenie zakończenia pracy klienta
  STOP,
  // Sprawdzenie, czy klient jest aktywny (ping)
  ALIVE,
} MessageType;

typedef struct {
  // typ wiadomości
  MessageType type;
  // nadawca wiadomości
  char sender[MAX_NAME_LEN];
  // odbiorca wiadomości (jeśli dotyczy konkretnego klienta)
  char receiver[MAX_NAME_LEN];
  // treść wiadomości
  char message[MAX_MSG_LEN];
  // znacznik czasu wysłania wiadomości
  time_t timestamp;
} Message;

// Funkcja do obsługi błędów (drukuje komunikat błędu i kończy
// działanie programu)
void error(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

#endif
