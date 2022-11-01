#include <am.h>
#include <nemu.h>
#include <stdio.h>

void __am_timer_init() {
}

//RTC_ADDR处的内存到底是怎么更新的
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  
  uptime->us = inl(RTC_ADDR + 4);
  uptime->us <<= 32;
  uptime->us |= inl(RTC_ADDR);
  //printf("%lld\n",uptime->us);
  // uptime->us = paddr_read(RTC_ADDR, 4);
  // uptime->us <<= 32;
  // uptime->us |= paddr_read(RTC_ADDR + 4, 4);
  //ioe_read(AM_TIMER_UPTIME, &uptime->us);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
