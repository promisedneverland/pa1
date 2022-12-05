#include <am.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      default: ev.event = EVENT_ERROR; break;
    }
    
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  printf("\n");
  return c;
}

extern void __am_asm_trap(void);

//mepc寄存器 - 存放触发异常的PC
//mstatus寄存器 - 存放处理器的状态
//mcause寄存器 - 存放触发异常的原因


//cte相关初始化，由init_irq调用
bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  //设置异常入口地址,直接将异常入口地址设置到mtvec寄存器中即可
  //mtvec : 0x305
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // 注册事件处理回调函数，回调函数由nanos-lite提供
  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

//用于进行自陷操作
void yield() {
  asm volatile("li a7, -1; ecall");
  //加载立即数，ecall引发环境调用异常
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
