#include <stdint.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  // printf("dummy\n");
  // return 0;
  _syscall_(SYS_yield, 0, 0, 0);
  return 0;
}
