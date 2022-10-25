#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  const char* i = s;
  size_t num = 0;
  while(*i != '\0')
  {
    num++;
    i++;
  }  
  return num;
}

char *strcpy(char *dst, const char *src) {
  size_t i;
  for(i = 0; src[i] != '\0'; i++)
    dst[i] = src[i];
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for(i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for(; i < n; i++)
    dst[i] = '\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t i;
  for (i = 0 ; src[i] != '\0' ; i++)
    dst[dst_len + i] = src[i];
  dst[dst_len + i] = '\0';
  return dst;
           
}

int strcmp(const char *s1, const char *s2) {
  int res = 0;
  unsigned char* u1 = (unsigned char*)s1;
  unsigned char* u2 = (unsigned char*)s2;
  for(int i = 0 ; (u1[i] != '\0' || u2[i] != '\0') ; i++)
  {
    if(u1[i] > u2[i])
    {
      res = 1;
      break;
    }
    if(u1[i] < u2[i])
    {
      res = -1;
      break;
    }
  }
  return res;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  int res = 0;
  unsigned char* u1 = (unsigned char*)s1;
  unsigned char* u2 = (unsigned char*)s2;
  for(int i = 0 ; (u1[i] != '\0' || u2[i] != '\0') && i < n ; i++)
  {
    if(u1[i] > u2[i])
    {
      res = 1;
      break;
    }
    if(u1[i] < u2[i])
    {
      res = -1;
      break;
    }
  }
  return res;
}

void *memset(void *s, int c, size_t n) {
  unsigned char* us = (unsigned char*)s;
  for(int i = 0; i < n ;i++)
  {
    us[i] = (unsigned char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char* us = (unsigned char*)src;
  unsigned char* ud = (unsigned char*)dst;
  unsigned char* tmp = (unsigned char*)malloc(n);
  for(int i = 0; i < n ;i++)
  {
    tmp[i] = us[i];
  }
  for(int i = 0; i < n ;i++)
  {
    ud[i] = tmp[i];
  }
  free(tmp);
  return dst;

}

void *memcpy(void *out, const void *in, size_t n) {
  unsigned char* ui = (unsigned char*)in;
  unsigned char* uo = (unsigned char*)out;
  for(int i = 0; i < n ;i++)
  {
    uo[i] = ui[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  int res = 0;
  unsigned char* u1 = (unsigned char*)s1;
  unsigned char* u2 = (unsigned char*)s2;
  for(int i = 0 ; i < n ; i++)
  {
    if(u1[i] > u2[i])
    {
      res = 1;
      break;
    }
    if(u1[i] < u2[i])
    {
      res = -1;
      break;
    }
  }
  return res;
}

#endif
