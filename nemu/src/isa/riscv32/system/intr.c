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
//mepc寄存器 - 存放触发异常的PC
//mstatus寄存器 - 存放处理器的状态
//mcause寄存器 - 存放触发异常的原因

//SR[mepc] <- PC
//SR[mcause] <- 一个描述失败原因的号码
//PC <- SR[mtvec]

//返回异常入口地址
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  //etrace
  // printf("etrace : mcause = %d, mepc = 0x%x, mtvec = 0x%x, mstatus = %d\n" , NO,epc,sr(mtvec),sr(mstatus));

  sr(mepc) = epc;
  sr(mcause) = NO;
  return sr(mtvec);
 
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
