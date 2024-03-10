#include <stdio.h>

#ifdef DYNAMIC

#include <dlfcn.h>

#else

#include "collatz.h"

#endif  // DYNAMIC

#define MAX_ITER 1000000

typedef int (*TestCollatzFunction)(int, int);

TestCollatzFunction get_test_collatz_function() {
#ifdef DYNAMIC
  void *handle = dlopen("./libcollatz_shared.so", RTLD_LAZY);
  if (!handle) {  // error
    printf("Errors with handle!\n");
    return NULL;
  }
  TestCollatzFunction test_collatz_function =
      dlsym(handle, "test_collatz_convergence");
  if (dlerror() != NULL) {  // error
    printf("Errors in dll!\n");
    return NULL;
  }
  return test_collatz_function;
#else
  return test_collatz_convergence;
#endif  // DYNAMIC
}

int main() {
  TestCollatzFunction test_collatz_convergence = get_test_collatz_function();
  int input;
  int result;

  if (test_collatz_convergence == NULL) return 1;

  scanf("%d", &input);
  result = test_collatz_convergence(input, MAX_ITER);
  printf("%d\n", result);

  return 0;
}