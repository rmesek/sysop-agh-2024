int collatz_conjecture(int input) {
  if (input % 2 == 0) return input / 2;
  return 3 * input + 1;
}

int test_collatz_convergence(int input, int max_iter) {
  for (int iter = 0; iter < max_iter; ++iter) {
    if (input == 1) return iter;
    input = collatz_conjecture(input);
  }
  return -1;
}