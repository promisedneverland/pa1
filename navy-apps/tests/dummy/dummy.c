#include <stdint.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  // printf("dummy\n");
  return 0;
  // return _syscall_(2, 0, 0, 0);
}
