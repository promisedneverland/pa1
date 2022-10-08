/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
int flag = 1;
const int maxdigit = 1;
const int overflow = 50000;
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s ; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
int idx = 0;
uint32_t choose(uint32_t n)
{

  return rand() % n;
}
void gen_num()
{
   
  buf[idx] = rand()%3+1+'0';
  idx++;
  // int length = rand() % maxdigit + 1;
  // for(int i=1;i<=length;i++)
  // {
  //   buf[idx] = rand() % 10 + '0';
  //   idx++;
  // }
  // buf[idx] = 'u';
  // idx++;
  if(idx > overflow)
  {
    flag = 0;
  }
}
void gen_rand_op()
{

  char opt;
  switch (choose(4)){
    case 0: opt = '+'; break;
    case 1: opt = '-'; break;
    case 2: opt = '*'; break;
    case 3: opt = '/'; break;
    default : printf("wrong"); break;
  }
  buf[idx] = opt;
  idx++;
}
void gen_space()
{
  int length = rand() % maxdigit + 1;
  for(int i=1;i<=length;i++)
  {
    buf[idx] = ' ';
    idx++;
  }
  
}
void gen(char c)
{
  buf[idx] = c;
  idx++;
}
static void gen_rand_expr() {
  if(flag == 0){
    gen_num();
    
    return ;
  }

  // gen_space();
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
  // buf[0] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);//loop 是 生成数据数量
  }
  int i;
  for (i = 0; i < loop; i ++) {
    flag = 1;
    gen_rand_expr();
    
    sprintf(code_buf, code_format, buf);
    
    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr -Werror=div-by-zero");//expr是可执行文件
    if (ret != 0){
      idx = 0;
      memset(buf,0,65536);
      continue;//不成功
    } 

    fp = popen("/tmp/.expr", "r");//fp 是 .expr 的输出
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
    idx = 0;
    memset(buf,0,65536);
  }
  return 0;
}
