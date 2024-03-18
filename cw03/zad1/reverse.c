// Zadanie 1 Napisz program, który kopiuje zawartość jednego pliku do drugiego,
// odwróconą bajt po bajcie.

// Wskazówki: Wywołania w rodzaju fseek(infile, +1024, SEEK_END) lub lseek(in,
// +1024, SEEK_END) są zupełnie legalne i nie powodują żadnych skutków
// ubocznych. Aby po przeczytaniu bloku znaków cofnąć się na początek
// poprzedniego bloku, należy jako drugi argument funkcji fseek(..., ...,
// SEEK_CUR) lub lseek(...., ..., SEEK_CUR) podać podwojoną długość bloku ze
// znakiem minus. Działanie programu należy zweryfikować następująco: 1)
// odwrócić krótki plik tekstowy, podejrzeć wynik, sprawdzić szczególnie
// początkowe i końcowe znaki. 2) ./reverse plik_binarny tmp1 ; ./reverse tmp1
// tmp2 ; diff -s tmp2 plik_binarny 3) można też porównać (diff -s) wynik
// działania programu i wynik polecenia tac < plik_wejściowy | rev >
// plik_wyjściowy
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 1024

void byte_reverse(FILE*, FILE*);
void block_reverse(FILE*, FILE*);

int main(int argc, char** argv) {
  FILE *input_f, *output_f;

  if (argc != 3) {
    printf("reverse <input_file> <output_file>\n");
    return -1;
  }

  input_f = fopen(argv[1], "r");
  if (input_f == NULL) {
    printf("Error while opening output file!\n");
    return -1;
  }
  output_f = fopen(argv[2], "w");
  if (output_f == NULL) {
    printf("Error while opening output file!\n");
    return -1;
  }

  block_reverse(input_f, output_f);

  fclose(input_f);
  fclose(output_f);
}

void byte_reverse(FILE* input_f, FILE* output_f) {
  char c;

  if (fseek(input_f, -1, SEEK_END) != 0) return;
  while (1) {
    c = fgetc(input_f);
    fprintf(output_f, "%c", c);

    if (fseek(input_f, -2, SEEK_CUR) != 0) return;
  }
}

void block_reverse(FILE* input_f, FILE* output_f) {
  char c;
  char block[BLOCK_SIZE];
  size_t num_read;
  size_t count = BLOCK_SIZE;

  fseek(input_f, -BLOCK_SIZE, SEEK_END);
  while (1) {
    num_read = fread(block, sizeof(char), count, input_f);

    // reverse block containg num_read items
    for (size_t i = 0; i < num_read / 2; ++i) {
      c = block[i];
      block[i] = block[num_read - 1 - i];
      block[num_read - 1 - i] = c;
    }
    fwrite(block, sizeof(char), num_read, output_f);

    if (num_read < BLOCK_SIZE) return;
    if (fseek(input_f, -2 * count, SEEK_CUR) != 0) {  // last block
      count = ftell(input_f) - BLOCK_SIZE;
      fseek(input_f, 0, SEEK_SET);
    }
  }
}