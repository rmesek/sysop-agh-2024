# Napisz program, który kopiuje zawartość jednego pliku do drugiego, odwróconą
# bajt po bajcie.

# Wskazówki: Wywołania w rodzaju fseek(infile, +1024, SEEK_END) lub lseek(in,
# +1024, SEEK_END) są zupełnie legalne i nie powodują żadnych skutków
# ubocznych. Aby po przeczytaniu bloku znaków cofnąć się na początek
# poprzedniego bloku, należy jako drugi argument funkcji fseek(..., ...,
# SEEK_CUR) lub lseek(...., ..., SEEK_CUR) podać podwojoną długość bloku ze
# znakiem minus. Działanie programu należy zweryfikować następująco: 1)
# odwrócić krótki plik tekstowy, podejrzeć wynik, sprawdzić szczególnie
# początkowe i końcowe znaki. 2) ./reverse plik_binarny tmp1 ; ./reverse tmp1
# tmp2 ; diff -s tmp2 plik_binarny 3) można też porównać (diff -s) wynik
# działania programu i wynik polecenia tac < plik_wejściowy | rev >
# plik_wyjściowy

CC=gcc-13
CFLAGS=-Wall -g

all: reverse

reverse.o: reverse.c
	$(CC) $(CFLAGS) reverse.c -c -o reverse.o

reverse: reverse.o
	$(CC) reverse.o -o reverse

.PHONY: all clean
clean:
		rm -f *.o reverse