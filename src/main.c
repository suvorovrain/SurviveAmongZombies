#include "utils/utils.h"
#include <stdio.h>

int main() {
  printf("5 + 3 = %d\n", add(5, 3));

#ifdef OS_WINDOWS
  printf("Running on Windows\n");
#elif defined(OS_LINUX)
  printf("Running on Linux\n");
#elif defined(OS_MACOS)
  printf("Running on macOS\n");
#endif

  return 0;
}