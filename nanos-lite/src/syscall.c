#include <common.h>
#include "syscall.h"
int sys_write(int fd, char *buf, size_t count);
void sys_yield()
{
  yield();
}
void *sys_sbrk(intptr_t addr);

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
int sys_gettimeofday(struct timeval *tv, struct timezone *tz);

void do_syscall(Context *c) {
  uintptr_t a[4];
  
  //通过宏GPR1从上下文c中获取用户进程之前设置好的系统调用参数
  a[0] = c->GPR1; // 异常号
  a[1] = c->GPR2; //第一个参数
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("strace : type = %d, gpr2 = %x, gpr3 = %x, gpr4 = %x\n\n", a[0],a[1],a[2],a[3]);
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
      c->GPRx = fs_write(a[1],(char*)a[2],a[3]);
      return ;
    }
    case SYS_brk:
    { 
      sys_sbrk(a[1]);
      c->GPRx = 0;
      return ;
    }
    case SYS_open:
    { 
      c->GPRx = fs_open((char*)a[1],a[2],a[3]);
      return ;
    }
    case SYS_lseek:
    { 
      c->GPRx = fs_lseek(a[1],a[2],a[3]);
      return ;
    }
    case SYS_read:
    { 
      c->GPRx = fs_read(a[1],(char*)a[2],a[3]);
      return ;
    }
    case SYS_close:
    { 
      c->GPRx = fs_close(a[1]);
      return ;
    }
    case SYS_gettimeofday:
    { 
      c->GPRx = sys_gettimeofday((struct timeval*)a[1],(struct timezone*)a[2]); 
      return ;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

// int sys_write(int fd, char *buf, size_t count)
// {
  
//   if(fd == 1 || fd == 2)
//   {
//     for(int i = 0 ; i < count ; i++)
//     {
//       putch(buf[i]);
//     }
//     return count;
//   }
//   return -1;
// }


void *sys_sbrk(intptr_t addr)
{
  return 0;
}