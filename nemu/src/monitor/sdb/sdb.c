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

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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
  return -1;
}

static int cmd_help(char *args);

static struct {//命令们
  const char *name;
  const char *description;
    int (*handler) (char *);//函数指针handler，参数是*char
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
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
  else {
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
