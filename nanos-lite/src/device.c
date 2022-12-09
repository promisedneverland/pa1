#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

// #key 把key转化成"key"
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

//len 一般为1024
size_t events_read(void *buf, size_t offset, size_t len) {

  assert(buf != NULL);
  //绝对不能io_read().keycode这么用 ？？？？？？
  printf("%x\n",buf);
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  int keycode = ev.keycode;
  if(keycode != AM_KEY_NONE)
  {
    // printf("press\n");
    bool keydown = ev.keydown;
    // printf("code %x keydown %d\n",keycode,keydown);
    char ckeydown = (keydown) ? 'd' : 'u';
    // char* cbuf = buf;
    // int length;
    // printf("k%c%s", ckeydown, keyname[keycode]);
    sprintf((char*)buf, "k%c%s", ckeydown, keyname[keycode]);
    printf("%s, strlen = %d\n",buf,strlen(buf));
    // sprintf((char*)buf + length, "asdasdasd");
    // length = sprintf((char*)buf + length + 1, "%s");
   
  }
  // else 
  //   sprintf((char*)buf, "\0");
  // printf("%d ",len);
  return len;
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
  // printf("%d sec, %d usec");
  tv->tv_sec = uptime / 1000000;
  tv->tv_usec = uptime % 1000000;
  return 0;
}
void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
