# Stwórz bibliotekę w języku C wystawiającą klientom następujące dwie funkcje:
# int collatz_conjecture(int input) - funkcja realizująca regułę Collatza
# postaci: Collatz Conjecture Funkcja ta przyjmuje jedną liczbę typu
# całkowitego. Jeżeli jest ona parzysta, podziel ją przez 2 i zwróć wynik.
# Jeżeli jest nieparzysta, pomnóż ją przez 3 i dodaj 1, a następnie zwróć
# wynik. int test_collatz_convergence(int input, int max_iter) - funkcja
# sprawdzająca po ilu wywołaniach collatz_conjecture zbiega się do 1. Powinna
# ona wywoływać regułę Collatza najpierw na liczbie wejściowej a później na
# wyniku otrzymanym z reguły. W celu ochrony przed zbyt długim zapętlaniem się
# funkcji drugi parametr stanowi górną granicę liczby iteracji. W przypadku gdy
# funkcja wykona maksymalną ilość iteracji i nie znajdzie wyniku 1, wtedy zwróć
# -1. W pliku makefile utwórz dwa wpisy: do kompilacji statycznej biblioteki i
# do kompilacji współdzielonej. Napisz klienta korzystającego z kodu
# biblioteki, klient powinien sprawdzać kilka liczb, wykorzystując
# test_collatz_convergence, tj. po ilu iteracjach wynik zbiegnie się do 1 i
# wypisać liczbę iteracji na standardowe wyjście. Klient powinien korzystać z
# biblioteki na 3 sposoby: Jako biblioteka statyczna Jako biblioteka
# współdzielona (linkowana dynamicznie) Jako biblioteka ładowana dynamicznie
# Dla każdego z wariantów utwórz odpowiedni wpis w Makefile. Do realizacji
# biblioteki dynamicznej użyj definicji stałej (-D) i dyrektywy preprocesora,
# aby zmodyfikować sposób działania klienta. Wyświetl zawartość plików
# binarnych wszystkich wariantów klienta przy pomocy programu objdump, znajdź
# miejsca wywołania funkcji test_collatz_convergence omów różnice w kodzie
# binarnym. Dla większej jasności kodu kompiluj bez optymalizacji -O0.

CC=gcc-13
CFLAGS=-Wall -g -O0

all: static shared dynamic

static-lib:
	$(CC) $(CFLAGS) -c collatz.c -o collatz_static.o
	ar rcs libcollatz_static.a collatz_static.o

shared-lib:
	$(CC) $(CFLAGS) -fPIC -c collatz.c -o collatz_shared.o
	$(CC) $(CFLAGS) -shared collatz_shared.o -o libcollatz_shared.so

#dynamic-lib:
#	$(CC) -fPIC -c -g collatz.c -o collatz_dynamic.o
#	$(CC) -shared collatz_dynamic.o -o libcollatz_dynamic.so

static: static-lib
	$(CC) $(CFLAGS) test_collatz.c libcollatz_static.a -o test_collatz_static

shared: shared-lib
	$(CC) $(CFLAGS) test_collatz.c -lcollatz_shared -L ./ -Wl,-rpath,. -o test_collatz_shared

dynamic: shared-lib  # same as shared "libcollatz_shared.so"
	$(CC) $(CFLAGS) test_collatz.c -ldl -DDYNAMIC -o test_collatz_dynamic

test: all
	@printf "static: "; ARG=27; echo $$ARG | ./test_collatz_static | xargs printf "$$ARG->%d\n"
	@printf "shared: "; ARG=27; echo $$ARG | ./test_collatz_shared | xargs printf "$$ARG->%d\n"
	@printf "dynamic: "; ARG=27; echo $$ARG | ./test_collatz_dynamic | xargs printf "$$ARG->%d\n"

.PHONY: clean static shared dynamic static-lib shared-lib 
clean:
	rm -f *.o *.a *.so test_collatz_static test_collatz_shared test_collatz_dynamic
