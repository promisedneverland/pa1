#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
// #include "NDL.h"

// uint32_t NDL_GetTicks();
int main() {
    // struct timeval old;
    // struct timeval new;
    // gettimeofday(&old,NULL); 
    // while(1)
    // {
    //     gettimeofday(&new,NULL); 
    //     if(new.tv_sec * 1000000 - old.tv_sec * 1000000 + new.tv_usec - old.tv_usec > 1000000)
    //     {
    //         old.tv_sec = new.tv_sec;
    //         old.tv_usec = new.tv_usec;
    //         printf("1sec\n");
    //     }
    // }
  NDL_Init();
  uint32_t old;
  uint32_t new;
  old = NDL_GetTicks();
  while(1){
    new = NDL_GetTicks();
    if(new - old > 1000)
    {
        old = new;
        printf("1sec?\n");
    }
  }
  return 0;
}
