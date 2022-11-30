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
//logfile相关
#include <common.h>

//客户程序的指令数
extern uint64_t g_nr_guest_inst;

//指向logfile 的文件指针
FILE *log_fp = NULL;//file = iofile

// log_file 示例 ： /home/lcx/ics2022/nanos-lite/build/nemu-log.txt
void init_log(const char *log_file) {
  log_fp = stdout;// output log_fp
  if (log_file != NULL) {
    //fopen创建一个用于写入的空文件。如果文件名称与已存在的文件相同，则会删除已有文件的内容，文件被视为一个新的空文件。
    FILE *fp = fopen(log_file, "w");//create a file named log-file
    //return NULL if error
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

//控制着Log宏是否会输出到logfile中，但不会控制Log宏是否会输出到终端
bool log_enable() {
  //目前定义了CONFIG_TRACE,因此下面的语句暂时是
  //return (g_nr_guest_inst >= 0) && (g_nr_guest_inst <= 10000)
  return MUXDEF(CONFIG_TRACE, (g_nr_guest_inst >= CONFIG_TRACE_START) &&
         (g_nr_guest_inst <= CONFIG_TRACE_END), false);
}
