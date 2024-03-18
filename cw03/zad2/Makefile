# Napisz program, który będzie przeglądał bieżący katalog, korzystając z
# funkcji opendir(), readdir() i stat(). Dla każdego znalezionego pliku, który
# nie jest katalogiem, czyli !S_ISDIR(bufor_stat.st_mode), należy wypisać
# rozmiar i nazwę pliku. Ponadto na koniec należy wypisać sumaryczny rozmiar
# wszystkich plików. Nie należy przeglądać podkatalogów! Sumaryczny rozmiar
# plików należy przechowywać w zmiennej typu long long i wypisywać ją przez
# format %lld.

# Działanie programu porównaj z działaniem polecenia wc --bytes *

CC=gcc-13
CFLAGS=-Wall -g

all: dir_size

dir_size.o: dir_size.c
	$(CC) $(CFLAGS) dir_size.c -c -o dir_size.o

dir_size: dir_size.o
	$(CC) dir_size.o -o dir_size

.PHONY: all clean
clean:
		rm -f *.o dir_size