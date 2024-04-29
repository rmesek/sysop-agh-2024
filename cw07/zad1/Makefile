# Napisz prosty program typu klient-serwer, w którym komunikacja zrealizowana
# jest za pomocą kolejek komunikatów. Serwer po uruchomieniu tworzy nową
# kolejkę komunikatów. Za pomocą tej kolejki klienci będą wysyłać komunikaty do
# serwera. Komunikaty wysyłane przez klientów mogą zawierać polecenie
# oznaczające pierwsze nawiązanie połaczenia z serwerem (INIT) lub jeśli
# wcześniej połączenie zostało już nawiązane: identyfikator klienta wraz z
# komunikatem, który ma zostać przekazany przez serwer do wszystkich
# pozostałych klientów. W odpowiedzi na polecenie INIT, serwer ma przesłać
# identyfikator nadany nowemu klientowi. Klient bezpośrednio po uruchomieniu
# tworzy kolejkę z unikalnym kluczem IPC i wysyła jej klucz do serwera wraz z
# komunikatem INIT. Po otrzymaniu takiego komunikatu, serwer otwiera kolejkę
# klienta, przydziela klientowi identyfikator (np. numer w kolejności zgłoszeń)
# i odsyła ten identyfikator do klienta (komunikacja w kierunku serwer->klient
# odbywa się za pomocą kolejki klienta). Po otrzymaniu identyfikatora, klient
# może wysłać do serwera komunikaty, które serwer będzie przesyłał do
# wszystkich pozostałych klientów. Komunikaty te są czytane ze standardowego
# wejścia. Klient po uruchomieniu tworzy drugi proces, który powinien odbierać
# komunikaty wysyłane przez serwer (przy użyciu kolejki komunikatów klienta) i
# wyświetlać te komunikaty na standardowym wyjściu.

# Klient i serwer należy napisać w postaci osobnych programów. Serwer musi być
# w stanie pracować jednocześnie z wieloma klientami. Dla uproszczenia można
# przyjąć, że serwer przechowuje informacje o klientach w statycznej tablicy
# (rozmiar tablicy ogranicza liczbę klientów, którzy mogą się zgłosić do
# serwera).

# Powyższe zadanie można zrealizować wykorzystując mechanizmy System V lub
# POSIX.

CC=gcc-13
CFLAGS=-Wall -g

all: chat_client chat_server

chat_client.o: chat_client.c
	$(CC) $(CFLAGS) chat_client.c -c -o chat_client.o

chat_client: chat_client.o
	$(CC) chat_client.o -o chat_client

chat_server.o: chat_server.c
	$(CC) $(CFLAGS) chat_server.c -c -o chat_server.o

chat_server: chat_server.o
	$(CC) chat_server.o -o chat_server

.PHONY: all clean
clean:
		rm -f *.o chat_client chat_server