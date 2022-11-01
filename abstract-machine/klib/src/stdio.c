#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

void itoa(long long integer, char* out, int base);//only base = 10 is valid
void printstr(const char* str);
void swap(char* a, char* b);
#define PRINT_BUF_SIZE 65536
#define PRINTF_HANDLE(charout,out) { \
  va_list ap; \
  int d; \
  long long l;\
  char c; \
  char *s; \
  char buffer[PRINT_BUF_SIZE]; \
  va_start(ap, fmt); \
  while (*fmt) \
  {\
    if(*fmt == '%')\
    {\
      fmt++;\
      switch (*fmt) {\
      case 's':\
        s = va_arg(ap, char *);\
        strcpy(out+charout,s);\
        charout += strlen(s) + 1;\
        break;\
      case 'l':\
        l = va_arg(ap, long long);\
        itoa(l,buffer,10);\
        strcpy(out+charout,buffer);\
        charout += strlen(buffer) ;\
        break;\
      case 'd':\
        d = va_arg(ap, int);\
        itoa(d,buffer,10);\
        strcpy(out+charout,buffer);\
        charout += strlen(buffer) ;\
        break;\
      case 'c':\
        c = (char) va_arg(ap, int);\
        out[charout] = c;\
        charout++;\
        break;\
      }\
      fmt++;\
    }\
    else\
    {\
      out[charout] = *fmt;\
      fmt++;\
      charout++;\
    }\
  }\
  va_end(ap);\
  out[charout] = '\0';\
}

//__NATIVE_USE_KLIB__即为用自己写的KLIB
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
void printstr(const char* str)
{
  for(int i = 0 ; *(str + i) != '\0' ; i++)
  {
    putch(*(str + i));
  }
  //putch('\n');
}
int printf(const char *fmt, ...) {
  // char* str_tobe_print;
  // va_list ap;
  // va_start(ap, fmt);
  // int len = sprintf(str_tobe_print, fmt, ...);
  // printstr(str_tobe_print);
  // return len;
  int charout = 0;
  char out[PRINT_BUF_SIZE];
  
  PRINTF_HANDLE(charout,out);

  printstr(out);
  return charout;
}
void swap(char* a, char* b)
{
  char tmp = *a;
  *a = *b;
  *b = tmp;

}
int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}
void itoa(long long integer, char* out, int base)//only base = 10 is valid
{
  int charnum = 0, start = 0;
  if(integer < 0)
  {
    out[0] = '-';
    charnum = 1; 
    start = 1;
    integer = - integer;
  }

  //if(integer == (1 << 63)) todo
  if(integer == 0)
  {
    out[0] = '0';
    charnum++;
  }
  else
  {
    while(integer > 0)
    {
      out[charnum] = (integer % base) + '0';
      integer /= base; 
      charnum++;
    }
    
    for(int i = start; i < charnum - i - 1 + start; i++)
    {
      swap(&out[i], &out[charnum - i - 1 + start]);
    }
  }
  out[charnum] = 0;
}

int sprintf(char *out, const char *fmt, ...) {
  //panic("please implement me");

  // int charout = 0;
  
  // PRINTF_HANDLE(charout,out);
  int charout = 0;
  va_list ap;
  int d;
  char c;
  char *s;
  long long l;
  char buffer[PRINT_BUF_SIZE];
  va_start(ap, fmt);
  printstr(fmt);
  while (*fmt)
  {
    if(*fmt == '%')
    {
      fmt++;
      switch (*fmt) {
      case 's':              /* string */
        s = va_arg(ap, char *);
        strcpy(out+charout,s);
        charout += strlen(s) + 1;//\0
        break;
      case 'l':
        l = va_arg(ap, long long);
        itoa(l,buffer,10);
        strcpy(out+charout,buffer);
        charout += strlen(buffer) ;
        break;\
      case 'd':              /* int */
        //putch('d');
        //putch(':');
        d = va_arg(ap, int);
        itoa(d,buffer,10);
        //printstr("buffer = ");
        //printstr(buffer);
        strcpy(out+charout,buffer);
        //printstr("out = ");
        //printstr(out);
        charout += strlen(buffer) ;
        break;
      case 'c':              /* char */
        /* need a cast here since va_arg only takes fully promoted types */
        c = (char) va_arg(ap, int);
        out[charout] = c;
        charout++;
        break;
      }
      fmt++;
    }
    else
    {
      out[charout] = *fmt;
      fmt++;
      charout++;
    }
  }
  va_end(ap);
  out[charout] = '\0';
  
  //printstr(out);
  //printf("12345\n");
  return charout;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
