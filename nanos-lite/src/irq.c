#include <common.h>

static Context* do_event(Event e, Context* c) {
  c->mepc += 4;
  switch (e.event) {
    case EVENT_YIELD: 
    {
      printf("event = EVENT_YIELD\n");
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
