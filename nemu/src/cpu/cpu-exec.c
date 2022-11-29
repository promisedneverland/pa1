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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10000
#define I_RING_BUF_SIZE 64
#define I_RING_INS_SIZE 12
CPU_state cpu = {};//创建一个cpu
// word_t gpr[32];
//  vaddr_t pc;//PC

char iringbuf[I_RING_INS_SIZE][I_RING_BUF_SIZE];
int curIringIns = 0;
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;
bool check_wp();
void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  //printf("\n");
  //itrace
  IFDEF(CONFIG_ITRACE, sprintf(iringbuf[curIringIns],"%s",_this->logbuf));
  // if(nemu_state.state != NEMU_RUNNING)//add
  // {
  //   for(int i = 0; i < I_RING_INS_SIZE ; i++)
  //   {
  //     if(i == curIringIns)
  //       printf("->%4d  ",i);
  //     else
  //       printf("%6d  ",i);
  //     puts(iringbuf[i]);
  //   }
  // }
  // curIringIns++;
  // if(curIringIns == I_RING_INS_SIZE)
  //   curIringIns = 0;
  
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));

  if(!check_wp())
  {
    nemu_state.state = NEMU_STOP;
  }
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);//修改snpc为下一条指令的pc
  //snpc = dnpc= pc+4
  cpu.pc = s->dnpc;//根据dnpc 更新 pc

#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;//instrunction length
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
  //反汇编
#endif
}

//执行n条指令
static void execute(uint64_t n) {
  Decode s;

  for (;n > 0; n --) {
    //执行一条指令
    exec_once(&s, cpu.pc);

    //记录客户指令计数器+1
    g_nr_guest_inst ++;

    //difftest检查寄存器 + 输出执行的命令
    trace_and_difftest(&s, cpu.pc);

    //检查state，如果不是RUNNING就不再继续执行指令
    if (nemu_state.state != NEMU_RUNNING)
      break;

    //设备更新
    IFDEF(CONFIG_DEVICE, device_update());
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

//输出寄存器，运行数据信息
void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
//传入执行的指令数
void cpu_exec(uint64_t n) {

  //是否打印客户指令,如果太多就不打印了
  g_print_step = (n < MAX_INST_TO_PRINT);//0

  //检查nemu是否已经运行结束或终止,否则将state改为running
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }
  //nemu.state : STOP->RUNNING

  //获取执行前时间
  uint64_t timer_start = get_time();

  execute(n);//执行n次
  //nemu.state RUNNING->END

  //获取执行后时间
  uint64_t timer_end = get_time();

  //计算出此次客户程序n条指令运行的时间
  g_timer += timer_end - timer_start;//计算执行时间

  //若state 为 running, 则设置 state 为stop, 等待下一次启动
  //若为END或ABORT,则输出state，并根据停止时返回值判断GOOD/BAD TRAP ， 输出运行统计数据
  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;
    
    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);//调了色
      //printf("$a0 = %d\n",cpu.gpr[10]);//我加的
      // fall through
    case NEMU_QUIT: statistic();
  }
}
