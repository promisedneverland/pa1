#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

// int _gettimeofday(struct timeval *tv, struct timezone *tz);//自定义

//// 以毫秒为单位返回系统时间
uint32_t NDL_GetTicks() {
  struct timeval tm;
  gettimeofday(&tm,NULL);
  return (tm.tv_sec * 1000000 + tm.tv_usec) / 1000; 
}
extern int open(const char *path, int flags, ...);
// 读出一条事件信息, 将其写入`buf`中, 最长写入`len`字节
// 若读出了有效的事件, 函数返回1, 否则返回0
int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "r+");
  assert(fp);
  fseek(fp, 0, SEEK_SET);

  // fscanf(fp, "%s", (char*)buf);
  // int fd = open("/dev/events",0,0);
  fread(buf,1,len,fp);
  // read(fd,buf,len);
  // printf("navy\n");

  // fclose(fp);
  // if(strlen(buf))
  //   printf("%d\n",strlen(buf));
  return strlen(buf);
}

// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
void NDL_OpenCanvas(int *w, int *h) {
  //如果运行环境是NWM_APP
  if (getenv("NWM_APP")) {
    int fbctl = 4;//frame buffer control 的文件号
    fbdev = 5; // frame buffer 文件号

    //画布宽w，高h
    // let NWM resize the window and create the frame buffer
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    write(fbctl, buf, len);

    while (1) {
      // 3 = evtdev
      //在3号event文件中查找mmap ok，如果存在则表明完成画布的初始化工作
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
