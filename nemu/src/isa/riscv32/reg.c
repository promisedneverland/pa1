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

#include <isa.h>
#include "local-include/reg.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};
extern int used_rs1;
extern int used_rd;
extern int used_rs2;
void isa_reg_display() {//printf 输出所有寄存器的值
  for(int i=0;i<32;i++)
  {
    
    printf(RED "%5s 0x%08x %u\n" RESET,regs[i],cpu.gpr[i],cpu.gpr[i]);
  }  
}

word_t isa_reg_str2val(const char *s, bool *success) {
  //它用于返回名字为s的寄存器的值, 并设置success指示是否成功.
  for(int i=0;i<32;i++)
  {
    if(strcmp(regs[i],s) == 0)
    {
      *success = 1;
      return cpu.gpr[i];
    }
    
  }  
  *success = 0;
  return 0;
}
