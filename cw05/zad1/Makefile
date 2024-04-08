# Napisz program demonstrujący różne reakcje na przykładowy sygnał SIGUSR1 w
# zależności od ustawionych dyspozycji. Reakcja na sygnał SIGUSR1 programu
# powinna zależeć od wartości argumentu z linii poleceń. Argument ten może
# przyjmować wartości: none, ignore, handler, mask. Program w zależności od
# parametru odpowiednio: nie zmienia reakcji na sygnał, ustawia ignorowanie,
# instaluje handler obsługujący sygnał (działający w ten sposób, że wypisuje
# komunikat o jego otrzymaniu), maskuje ten sygnał oraz sprawdza czy
# wiszący/oczekujący sygnał jest widoczny. Następnie przy pomocy funkcji raise
# wysyła sygnał do samego siebie oraz wykonuje odpowiednie dla danej opcji,
# opisane wyżej działania.

CC=gcc-13
CFLAGS=-Wall -g

all: sigusr1_demo

sigusr1_demo.o: sigusr1_demo.c
	$(CC) $(CFLAGS) sigusr1_demo.c -c -o sigusr1_demo.o

sigusr1_demo: sigusr1_demo.o
	$(CC) sigusr1_demo.o -o sigusr1_demo

.PHONY: all clean
clean:
		rm -f *.o sigusr1_demo