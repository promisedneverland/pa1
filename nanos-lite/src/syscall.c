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
  a[0] = c->GPR1;
  a[0] = c->GPR1;
  a[0] = c->GPR1;
  switch (a[0]) {
    case 1:
    {
      sys_yield();
      c->GPRx = 0;
      return;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
