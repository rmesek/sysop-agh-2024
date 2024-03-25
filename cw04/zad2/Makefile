# Napisz program, który przyjmuje jeden argument: argv[1] — ścieżkę katalogu.
# Program powinien wypisać na standardowym wyjściu swoją nazwę, korzystając z
# funkcji printf(). Zadeklaruj zmienną globalną global, a następnie zmienną
# lokalną local. W zależności od zwróconej wartości przez fork() dokonaj
# obsługi błędu, wykonaj proces rodzica / proces potomny. W procesie potomnym:
# wyświetl komunikat „child process”,
# dokonaj inkrementacji zmiennych global i local,
# wyświetl komunikat „child pid = %d, parent pid = %d”
# wyświetl komunikat „child's local = %d, child's global = %d”
# wykonaj program /bin/ls z argumentem argv[1], korzystając z funkcji execl(),
# zwracając przy tym jej kod błędu. W procesie rodzica: wyświetl komunikat
# „parent process” wyświetl komunikat “parent pid = %d, child pid = %d”
# wyświetl komunikat “Child exit code: %d”
# wyświetl komunikat “Parent's local = %d, parent's global = %d”
# zwróć stosowny kod błędu.

CC=gcc-13
CFLAGS=-Wall -g

all: fork_exec

fork_exec.o: fork_exec.c
	$(CC) $(CFLAGS) fork_exec.c -c -o fork_exec.o

fork_exec: fork_exec.o
	$(CC) fork_exec.o -o fork_exec

.PHONY: all clean
clean:
		rm -f *.o fork_exec