# Napisz program, który przyjmuje jeden argument: argv[1]. Program ma utworzyć
# argv[1] procesów potomnych. Każdy proces potomny ma wypisać na standardowym
# wyjściu w jednym wierszu dwa identyfikatory: identyfikator procesu
# macierzystego i swój własny. Na końcu standardowego wyjścia proces
# macierzysty ma wypisać argv[1]. Wskazówka: aby program na pewno wypisywał
# argv[1] jako ostatni wiersz standardowego wyjścia, należy użyć funkcji
# systemowej wait().

CC=gcc-13
CFLAGS=-Wall -g

all: child

child.o: child.c
	$(CC) $(CFLAGS) child.c -c -o child.o

child: child.o
	$(CC) child.o -o child

.PHONY: all clean
clean:
		rm -f *.o child