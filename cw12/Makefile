# Zmodyfikuj rozwiązanie zadania z poprzedniego laboratorium w taki sposób aby
# komunikacja pomiędzy klientami a serwerem odbywała się z użyciem protokołu
# datagramowego

# Poprzednie zadanie.
# Napisz prosty chat typu klient-serwer w którym komunikacja pomiędzy 
# uczestmnikami czatu / klientami / klientami i serwerem realizoiwana 
# jest za pośrednictwem socketów z życiem protokołu strumieniowego.

# Adres / port serwera podawany jest jako argument jego uruchomienia

# Klient przyjmuje jako swoje argumenty:

# - swoją nazwę/identyfikator (string o z góry ograniczonej długości)
# - adres serwera (adres IPv4 i numer portu) 

# Protokół komunikacyjny powinien obsługiwać następujące operacje:

# LIST:
# Pobranie z serwera i wylistowanie wszystkich aktywnych klientów
# 2ALL string:
# Wysłania wiadomości do wszystkich pozostałych klientów. Klient wysyła 
# ciąg znaków do serwera, a serwer rozsyła ten ciąg wraz z identyfikatorem 
# nadawcy oraz aktualną datą do wszystkich pozostałych klientów
# 2ONE id_klienta string:
# Wysłanie wiadomości do konkretnego klienta. Klient wysyła do serwera 
# ciąg znaków podając jako adresata konkretnego klienta o identyfikatorze 
# z listy aktywnych klientów. Serwer wysyła ten ciąg wraz z identyfikatorem 
# klienta-nadawcy oraz aktualną datą do wskazanego klienta.
# STOP: Zgłoszenie zakończenia pracy klienta.  Powinno skutkować usunięciem 
# klienta z listy klientów przechowywanej na serwerze 
# ALIVE - serwer powinien cyklicznie "pingować" zarejestrowanych klientów, 
# aby zweryfikować że wciąż odpowiadają na żądania, a jeśli nie - usuwać 
# ich z listy klientów. Klient przy wyłączeniu Ctrl+C powinien wyrejestrować 
# się z serwera Dla uproszczenia można przyjąć, że serwer przechowuje 
# informacje o klientach w statycznej tablicy (rozmiar tablicy ogranicza 
# liczbę klientów, którzy mogą jednocześnie byc uczestnikami czatu).

CC=gcc-13
CFLAGS=-Wall -Wextra -pthread

all: chat_client chat_server

chat_client.o: chat_client.c
	$(CC) $(CFLAGS) chat_client.c -c -o chat_client.o

chat_client: chat_client.o
	$(CC) chat_client.o -o chat_client -lpthread

chat_server.o: chat_server.c
	$(CC) $(CFLAGS) chat_server.c -c -o chat_server.o

chat_server: chat_server.o
	$(CC) chat_server.o -o chat_server -lpthread

.PHONY: all clean
clean:
		rm -f *.o chat_client chat_server