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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include <generated/autoconf.h>
#include <macro.h>

//CONFIG_TARGET_AM 就是在nemu中用自己实现的klib
#ifdef CONFIG_TARGET_AM
#include <klib.h>
#else
#include <assert.h>
#include <stdlib.h>
#endif

#if CONFIG_MBASE + CONFIG_MSIZE > 0x100000000ul// unsinged long 8bytes
#define PMEM64 1//是否是64位的pmem，riscv32中不是
#endif

//word_t = uint32_t
typedef MUXDEF(CONFIG_ISA64, uint64_t, uint32_t) word_t;//如果定义isa64则字长为64
typedef MUXDEF(CONFIG_ISA64, int64_t, int32_t)  sword_t;

//FMT_WORD = "0x%08x"
#define FMT_WORD MUXDEF(CONFIG_ISA64, "0x%016"PRIx64, "0x%08"PRIx32)

//vaddr_t = word_t 
typedef word_t vaddr_t;

//定义paddr_t的数据类型
//paddr_t = uint32_t
typedef MUXDEF(PMEM64, uint64_t, uint32_t) paddr_t;

//FMT_PADDR 是paddr的格式化类型，用于printf; 
//目前是后面这种，FMT_PADDR = "0x%08x"，即最少8位，不够填充0
#define FMT_PADDR MUXDEF(PMEM64, "0x%016"PRIx64, "0x%08"PRIx32)//lx , x


typedef uint16_t ioaddr_t;

#include <debug.h>

#endif
