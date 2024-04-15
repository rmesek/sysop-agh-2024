# Napisz program, który liczy numerycznie wartość całki oznaczonej tak jak w
# zadaniu 1. Obliczenia powinny zostać przeprowadzone w ten sposób, że pierwszy
# program czyta ze standardowego wejścia przedział w jakim całka ma być
# policzona a następnie przesyła przez potok nazwany do programu drugiego
# wartości określające ten przedział. Drugi program po otrzymaniu informacji
# liczy całkę w otrzymanym przedziale i odsyła do programu pierwszego wynik
# obliczeń. Po otrzymaniu wyniku obliczeń, program pierwszy wyświetla wynik.

CC=gcc-13
CFLAGS=-Wall -g

all: integral_interval integral_calc

integral_interval.o: integral_interval.c
	$(CC) $(CFLAGS) integral_interval.c -c -o integral_interval.o

integral_interval: integral_interval.o
	$(CC) integral_interval.o -o integral_interval

integral_calc.o: integral_calc.c
	$(CC) $(CFLAGS) integral_calc.c -c -o integral_calc.o

integral_calc: integral_calc.o
	$(CC) integral_calc.o -o integral_calc

.PHONY: all clean
clean:
		rm -f *.o integral_interval integral_calc read_pipe.fifo write_pipe.fifo