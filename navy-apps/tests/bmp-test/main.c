#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>

int main() {
  NDL_Init(0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  // printf("bmp w = %d, h = %d\n",w,h);
  // w = 0;
  // h = 0;
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 10, 20, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
