#include <common.h>


void do_syscall(Context *c);
static Context* do_event(Event e, Context* c) {
  
  switch (e.event) {
    case EVENT_YIELD: 
    {
      printf("event = EVENT_YIELD\n\n");
      break;
    }
    case EVENT_SYSCALL:
    {
      printf("event = EVENT_SYSCALL\n");
      do_syscall(c); 
      break;
    }
    default: panic("Unhandled event ID = %d", e.event);
  }
  
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  //设置入口地址，注册回调函数do_event
  cte_init(do_event);

}
