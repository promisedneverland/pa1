#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  int i;
  int w = inw(VGACTL_ADDR + 2);  // TODO: get the correct width
  int h = inw(VGACTL_ADDR);  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = 0;
  //同步开
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(VGACTL_ADDR + 2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0,
  };
  //putch('\n');
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) 
  {
    //硬件同步开
    int w = inw(VGACTL_ADDR + 2);  // TODO: get the correct width
    int h = inw(VGACTL_ADDR);
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    for (int i = ctl->y; i <= ctl->y + h ; i ++)
    {
      for(int j = ctl->x; j <= ctl->x + w; j++)
        fb[j + i * w] = ((uint32_t *)ctl->pixels)[(j - ctl->x) + (i - ctl->y) * w];
    }
    outl(SYNC_ADDR, 1);
  }
  
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
