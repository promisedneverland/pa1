#include <common.h>
#include "syscall.h"

void sys_yield()
{
  yield();
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  
  //通过宏GPR1从上下文c中获取用户进程之前设置好的系统调用参数
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  printf("strace : type = %d, gpr2 = %x, gpr3 = %x, gpr4 = %x\n\n", a[0],a[1],a[2],a[3]);
  switch (a[0]) {
    case SYS_yield:
    {
      sys_yield();
      c->GPRx = 0;
      return;
    }
    case SYS_exit:
    {
      halt(a[1]);
      return ;
    }
    case SYS_write:
    { 

    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
