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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <common.h>
#include <stdio.h>
#include <utils.h>

//## __VA_ARGS__ 将被替换为...
// 内容解释:
// [src/utils/log.c:33 init_log] 中 src/utils/log.c:为__FILE__， 33为__LINE__， init_log 为 __func__
// #define ANSI_FMT(str, fmt) fmt str ANSI_NONE
// ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) 展开为  ANSI_FG_BLUE("\33[1;34m"[用于设置颜色]) "[%s:%d %s] " format ANSI_NONE(用于重置颜色)
#define Log(format, ...) \
    _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)
// ##__VA_ARGS__ 用于去除多余的逗号，当...为空白的时候

//不太明白为什么要根据CONFIG_TARGET有所区别
//输入判断条件和输出字符串(printf格式)；如果断言失败，则输出字符串，寄存器，运行数据信息
#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      MUXDEF(CONFIG_TARGET_AM, printf(ANSI_FMT(format, ANSI_FG_RED) "\n", ## __VA_ARGS__), \
        (fflush(stdout), fprintf(stderr, ANSI_FMT(format, ANSI_FG_RED) "\n", ##  __VA_ARGS__))); \
      IFNDEF(CONFIG_TARGET_AM, extern FILE* log_fp; fflush(log_fp)); \
      extern void assert_fail_msg(); \
      assert_fail_msg(); \
      assert(cond); \
    } \
  } while (0)

//panic 实际上触发了Assert(0),并输出错误信息
#define panic(format, ...) Assert(0, format, ## __VA_ARGS__)

#define TODO() panic("please implement me")

#endif
