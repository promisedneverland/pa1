#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
static int evtdev = -1;
static int fbdev = -1;
static int fbctr = -1;
static int dispinfo = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

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
  // FILE *fp = fopen("/dev/events", "r+");
  // assert(fp);
  // fseek(fp, 0, SEEK_SET);

  // fscanf(fp, "%s", (char*)buf);
  int fd = open("/dev/events",0,0);
  // fread(buf,1,len,fp);
  read(fd,buf,len);
  // printf("navy\n");
  close(fd);
  // fclose(fp);
  // if(strlen(buf))
  //   printf("%d\n",strlen(buf));
  return strlen(buf);
}

// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
static void parse(char* buf)
{
  int cur = 0;
  while(buf[cur])
  {
    if(buf[cur] == 'H')
    {
      while(buf[cur] > '9' || buf[cur] < '0')
        cur++;
      screen_h = atoi(buf + cur);
    }
    if(buf[cur] == 'W')
    {
      while(buf[cur] > '9' || buf[cur] < '0')
        cur++;
      screen_w = atoi(buf + cur);
    }
    cur++;
  }
  // printf("screen_w = %d, h = %d\n",screen_w,screen_h);
}

void NDL_OpenCanvas(int *w, int *h) {
  
  dispinfo = open("/proc/dispinfo",0,0);
  // fbdev = open("/dev/fb",0,0);

  // evtdev = 3;

  char buf[64];
  
  read(dispinfo, buf, 64);
  parse(buf);
  canvas_x = 0;
  canvas_y = 0;
  if(*w == 0 && *h == 0)
  {
    *h = screen_h;
    *w = screen_w;
  }
  
  canvas_h = *h;
  canvas_w = *w;
  
  
  // printf("%s\n",buf);
  // printf("canvas w = %d , h = %d\n",*w,*h);
  
  close(dispinfo);
  // close(fbdev);
  //默认不进入这个分支
  
  if (getenv("NWM_APP")) {
    printf("is NWM APP\n");
    int fbctl = 4;//frame buffer control 的文件号
    fbdev = 5; // frame buffer 文件号
    // int dispinfo = 6;
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
// 这里的pixels只是矩形的
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  // read();
  // printf("size of pixels = %d\n",w*h);
  char buf[64];
  // char temp[65500];
  fbdev = open("/dev/fb",0,0);
  fbctr = open("/dev/fbctr",0,0);
  // screen_w = 1600;
  //用am native 打开的屏幕宽度是1600
  // printf("x = %d,y = %d, w = %d , h = %d\n",x,y,w,h);
  sprintf(buf, "%d %d\n", w , h);
  write(fbctr, buf, strlen(buf));
  // printf("offset = %d\n",4 * (canvas_x + x + (canvas_y + y) * screen_w));
  // for(int i = 0 ; i < h ; i++)
  // {
  //   // printf("pos = %d\n",(canvas_x + x + (i + canvas_y + y) * screen_w));
  //   memcpy(temp, pixels + , w * 4);

    
  // }
  // lseek(fbdev, 4 * (canvas_x + x + (i + canvas_y + y) * screen_w) , SEEK_SET);
  // write(fbdev, temp , w * 4);
  // for(int i = 0 ; i < h ; i++)
  // {
  //   // printf("pos = %d\n",(canvas_x + x + (i + canvas_y + y) * screen_w));
  //   lseek(fbdev, 4 * (canvas_x + x + (i + canvas_y + y) * screen_w) , SEEK_SET);
  //   write(fbdev, pixels + i * w, w * 4);
  // }
  lseek(fbdev, 4 * (canvas_x + x + (canvas_y + y) * screen_w) , SEEK_SET);
  write(fbdev, pixels, w * h * 4);
  
  close(fbctr);
  close(fbdev);
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
