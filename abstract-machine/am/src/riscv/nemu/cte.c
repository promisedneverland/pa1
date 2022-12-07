#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

//初始为do_event
static Context* (*user_handler)(Event, Context*) = NULL;

//从am_asm_trap中直接跳转过来
Context* __am_irq_handle(Context *c) {
  // printf("mstatus = %d\n",c->mstatus);
  // printf("mepc = %d\n",c->mepc);
  // printf("mcause = %d\n",c->mcause);
  // printf("pdir = %d\n",c->pdir);
  //注册了回调函数user_handler
  if (user_handler) {
    Event ev = {0};
    
    if(c->GPR1 == -1)
    {
      ev.event = EVENT_YIELD; 
    }
    else if(c->GPR1 >= 0 && c->GPR1 <= 19)
    {
      ev.event = EVENT_SYSCALL; 
    }
    else 
    {
      ev.event = EVENT_ERROR;
    }
    // switch (c->mcause) {
    //   case -1: 
    //   {
    //     ev.event = EVENT_YIELD; 
    //     break;
    //   }
    //   case 1: 
    //   {
    //     ev.event = EVENT_SYSCALL; 
    //     break;
    //   }
    //   default: ev.event = EVENT_ERROR; break;
    // }
    //调用回调函数
    c -> mepc += 4;
    // printf("__am_irq_handle\n");
    //操作系统的事件分发函数do_event
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

//传入 do_event
bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  //设置异常入口地址
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  //注册事件处理回调函数，由nanos提供
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

//自限指令
void yield() {
  //为什么要li
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
