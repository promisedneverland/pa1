#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

void itoa(int integer, char* out, int radix);//only base = 10 is valid
void printstr(const char* str);
void swap(char* a, char* b);
#define PRINT_BUF_SIZE 6550
static const int N = 2147483647;


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
  char out[PRINT_BUF_SIZE];
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  printstr(out);
  return ret;
}

void swap(char* a, char* b)
{
  char tmp = *a;
  *a = *b;
  *b = tmp;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, N, fmt, ap);
}
// void itoa(long long integer, char* out, int base)//only base = 10 is valid
// {
//   int charnum = 0, start = 0;
//   if(integer < 0)
//   {
//     out[0] = '-';
//     charnum = 1; 
//     start = 1;
//     integer = - integer;
//   }

//   //if(integer == (1 << 63)) todo
//   if(integer == 0)
//   {
//     out[0] = '0';
//     charnum++;
//   }
//   else
//   {
//     while(integer > 0)
//     {
//       out[charnum] = (integer % base) + '0';
//       integer /= base; 
//       charnum++;
//     }
    
//     for(int i = start; i < charnum - i - 1 + start; i++)
//     {
//       swap(&out[i], &out[charnum - i - 1 + start]);
//     }
//   }
//   out[charnum] = 0;
// }

int sprintf(char *out, const char *fmt, ...) {
  //panic("please implement me");
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;


}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *final, size_t n, const char *fmt, va_list ap) {
  char* out;
  int charout = 0;
  int d; 
  long long l;
  char c; 
  char *s; 
  char * buffer; 
  // while (*fmt) 
  // {
  //   if(*fmt == '%')
  //   {
  //     fmt++;
  //     switch (*fmt) {
  //     case 's': 
  //       s = va_arg(ap, char *); 
  //       strcpy(out + charout, s); 
  //       charout += strlen(s); 
  //       break; 
  //     case 'l': 
  //       l = va_arg(ap, long long); 
  //       itoa(l,buffer,10); 
  //       strcpy(out + charout, buffer); 
  //       charout += strlen(buffer) ; 
  //       break; 
  //     case 'd': 
  //       d = va_arg(ap, int); 
  //       itoa(d,buffer,10); 
  //       strcpy(out + charout, buffer); 
  //       charout += strlen(buffer) ; 
  //       break; 
  //     case 'c': 
  //       if(charout < n) {  
  //         c = (char) va_arg(ap, int); 
  //         out[charout] = c; 
  //         charout++; 
  //       } 
  //       break; 
  //     case 'p': 
  //       d = va_arg(ap, int); 
  //       itoa(d,buffer,16); 
  //       strcpy(out + charout, buffer); 
  //       charout += strlen(buffer) ; 
  //       break; 
  //     case 'x': 
  //       d = va_arg(ap, int); 
  //       itoa(d,buffer,16); 
  //       strcpy(out + charout, buffer); 
  //       charout += strlen(buffer) ; 
  //       break; 
  //     } 
  //     fmt++; 
  //   } 
  //   else  
  //   { 
  //     out[charout] = *fmt; 
  //     fmt++; 
  //     charout++; 
  //   } 
  // } 
  // out[charout] = '\0'; 

  for(int i = 0 ; i <= charout; i++)
    final[i] = out[i];
  
  return charout;
}

#endif
