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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/paddr.h>
#include <utils.h>
// #include <expr.c>
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {//只要不输入q,就会一直进入
  //printf("rl gets\n");
  static char *line_read = NULL;
  //？一定为空
  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {//cpu执行
  cpu_exec(-1);

  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;//add
  return -1;//program wont end if =0

}
static int cmd_i(char *args) {
  if(args == NULL)
  {
    printf("arguments??? \n");
    return 0;
  }
  
  if(strcmp(args,"r")==0)
    isa_reg_display(); //printf 输出所有寄存器的值
  else if(strcmp(args,"w")==0)
  {
    ;
  }
  return 0;//program wont end if =0

}

static int cmd_x(char *args) {
  if(args == NULL)
  {
    printf("arguments??? \n");
    return 0;
  }  
  char* firstarg = strtok(args," ");
  char* secondarg = strtok(NULL," ");
  // printf("%s\n",firstarg);
  // printf("%s\n",secondarg);
  u_int32_t N = atoi(firstarg);//字节数
  // printf("N=%d\n",N);
  
  paddr_t start;//开始地址
  // printf("%d\n",sscanf(secondarg,"0x%8x",&start));
  if(secondarg == NULL || sscanf(secondarg,"0x%8x",&start)<=0)
  {
    printf("arguments??? \n");
    return 0;
  } 
  else
  {
     while(N--)
     {
      printf("0x%08x         0x%08x   %u\n",start,paddr_read(start,4),paddr_read(start,4));
      start += 4;
     }
  }
  // printf("%x",start);
  return 0;
}
static int cmd_s(char *args) {//单步
  if(args)
  {
    cpu_exec(atoi(args));
  }
  else
    cpu_exec(1);
  return 0;  
}

static int cmd_help(char *args);

static int cmd_p(char *args)
{
  bool state = 1;
  printf("%u\n",expr(args,&state));
  // make_token(args);
  return 0;
}

static struct {//命令们
  const char *name;
  const char *description;
    int (*handler) (char *);//函数指针handler，参数是*char
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "step instruction", cmd_s },
  { "info", "information reg/watchpoint", cmd_i },
  { "x", "scan memory N*(4bytes) startindex", cmd_x },
  { "p", "calculate expression", cmd_p },
  //命令 描述 函数
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)//array length????

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given ， 打印所有指令和信息*/
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {//给了参数，打印指定指令
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {//什么时候set batch mode 了
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) 
  {
    char *str_end = str + strlen(str);//指向字符串末尾指针

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");//cmd指向第一个字符，下次调用从第一个空格之后开始
    if (cmd == NULL) { continue; }//若根本没有字符

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;//指向参数的第一个字符
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++) {//扫描是否是某个命令
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0)
          { return; }//执行命令
        break;
      }
    }
    //不匹配任何已知命令
    if (i == NR_CMD)
      { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
