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

#include <common.h>
// u_int32_t result,expect;
// bool success;
// char expression[65536+128];
// word_t expr(char *e, bool *success);
void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();

int is_exit_status_bad();

int main(int argc, char *argv[]) {
  // FILE *fp = popen("/home/lcx/ics2022/nemu/tools/gen-expr/input", "r");
  // assert(fp != NULL);
  // while(fscanf(fp, "%d%s", &result,expression) != EOF){
  //   expect = expr(expression,&success);
  //   if(!success)
  //     printf("failed\n");
  //   if(result != expect){
  //     printf("WRONG answer , expression = %s\n , should = %u \n, but = %u\n ",expression,result,expect);
  //   }
  // }
  
  // pclose(fp);
  // return 0;
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
