#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
int main() {
    struct timeval old;
    struct timeval new;
    gettimeofday(&old,NULL); 
    while(1)
    {
        gettimeofday(&new,NULL); 
        if(new.tv_sec * 1000 - old.tv_sec * 1000 + new.tv_usec - old.tv_usec > 1000)
        {
            old.tv_sec = new.tv_sec;
            old.tv_usec = new.tv_usec;
            printf("0.5sec\n");
        }
    }
  
  return 0;
}
