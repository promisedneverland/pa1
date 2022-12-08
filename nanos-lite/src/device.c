#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i = 0 ; i < len; i++)
  {
    putch(*((char*)buf + i));
  }
  return len;
 
}

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}


int sys_gettimeofday(struct timeval *tv, struct timezone *tz) 
{
  long long uptime;//微秒数
  ioe_read(AM_TIMER_UPTIME,&uptime);
  tv->tv_sec = uptime / 1000;
  tv->tv_usec = uptime % 1000;
  return 0;
}
void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
