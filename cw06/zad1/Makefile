# Napisz program, który liczy numerycznie wartość całki oznaczonej z funkcji
# 4/(x^2+1) w przedziale od 0 do 1 metodą prostokątów (z definicji całki
# oznaczonej Riemanna). Pierwszy parametr programu to szerokość każdego
# prostokąta, określająca dokładność obliczeń. Obliczenia należy rozdzielić na
# n procesów potomnych (n drugi parametr wywołania programu), tak by każdy z
# procesów liczył inny fragment ustalonego wyżej przedziału. Każdy z procesów
# powinien wynik swojej części obliczeń przesłać przez potok nienazwany do
# procesu macierzystego. Każdy proces potomny do komunikacji z procesem
# macierzystym powinien używać osobnego potoku. Proces macierzysty powinien
# oczekiwać na wyniki uzyskane od wszystkich procesów potomnych po czym
# powinien dodać te wyniki cząstkowe i wyświetlić wynik na standardowym
# wyjściu. W programie zmierz, wypisz na konsolę i zapisz do pliku z raportem
# czasy realizacji dla różnej liczby procesów potomnych oraz różnych
# dokładności obliczeń. Dokładności obliczeń należy dobrać w ten sposób by
# obliczenia trwały co najmniej kilka sekund.

CC=gcc-13
CFLAGS=-Wall -g

all: integral

integral.o: integral.c
	$(CC) $(CFLAGS) integral.c -c -o integral.o

integral: integral.o
	$(CC) integral.o -o integral

test: all
	time ./integral 0.000000001 1
	time ./integral 0.000000001 8

.PHONY: all clean
clean:
		rm -f *.o integral