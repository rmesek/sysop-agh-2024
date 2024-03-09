# Zapoznaj się z koncepcją plików Makefile:
# https://www.gnu.org/software/make/manual/html_node/Introduction.html
# Zainstaluj/skonfiguruj IDE, w którym będziesz pracować przez resztę
# laboratoriów. (np. VS Code, Vim, etc.) Stwórz nowy projekt w IDE. Napisz
# prosty program countdown.c, który będzie w pętli odliczał od 10 do 0 i
# wypisywał aktualną liczbę na konsolę (każda liczba w nowej linii). Stwórz
# plik Makefile, za pomocą którego skompilujesz swój program. Makefile powinien
# zawierać co najmniej trzy targety: all, countdown, clean.

# all — powinien budować wszystkie targety (na razie tylko countdown, w
# kolejnych zadaniach będziemy dodawać nowe targety). countdown — buduje
# program countdown.c clean — usuwa wszystkie pliki binarne, czyści stan
# projektu

# Użyj zmiennych CC oraz CFLAGS do zdefiniowania kompilatora (gcc) i flag
# compilacji (-Wall, -std=c17, ...). Dodatkowo w Makefile powinien być
# specjalny target .PHONY. Skompiluj i uruchom program. Korzystając z gdb (lub
# lldb) zademonstruj poniższe: zatrzymywanie programu (breakpoint) wewnątrz
# pętli podejrzenie aktualnego indeks pętli kontynuację wykonywania programu
# wypisanie kolejnego indeksu
# usunięnie breakpoint-a
# kontynuowanie działania programu do końca


CC=gcc-13
CFLAGS=-c -Wall -g
all: countdown

countdown: countdown.o
	$(CC) countdown.o -o countdown

countdown.o: countdown.c
	$(CC) $(CFLAGS) countdown.c

.PHONY: clean
clean:
	rm -f *.o countdown
