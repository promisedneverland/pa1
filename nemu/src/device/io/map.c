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
#include <memory/host.h>
#include <memory/vaddr.h>
#include <device/map.h>

//物理设备空间大小
#define IO_SPACE_MAX (2 * 1024 * 1024)

//物理设备空间头部
static uint8_t *io_space = NULL;
//物理设备空间尾部
static uint8_t *p_space = NULL;

//申请size字节(对齐后实际上至少申请了一页空间)的物理设备空间,这些空间实际上在linux堆区中，返回新空间的起始地址
uint8_t* new_space(int size) {
  uint8_t *p = p_space;

  //页对齐,将低12位置零，高位加一
  size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;

  //更新p_space
  p_space += size;

  //检查是否超出了最大使用空间
  assert(p_space - io_space < IO_SPACE_MAX);

  //返回新空间的起始地址
  return p;
}

//检查addr是否在map范围
static void check_bound(IOMap *map, paddr_t addr) {
  if (map == NULL) {
    Assert(map != NULL, "address (" FMT_PADDR ") is out of bound at pc = " FMT_WORD, addr, cpu.pc);
  } else {
    Assert(addr <= map->high && addr >= map->low,
        "address (" FMT_PADDR ") is out of bound {%s} [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
        addr, map->name, map->low, map->high, cpu.pc);
  }
}

//调用回调函数，增加一步检查
static void invoke_callback(io_callback_t c, paddr_t offset, int len, bool is_write) {
  if (c != NULL) { c(offset, len, is_write); }//is_write 指示是否写数据
}

//在linux内存中用malloc申请设备空间 + 初始化设备空间的指针p_space,io_space
//io_space 指向设备空间的起始， p_space 指向当前使用空间末尾
void init_map() {
  //io_space 指向设备空间的起始
  io_space = malloc(IO_SPACE_MAX);//新建2MB设备空间
  assert(io_space);

  //p_space 指向当前使用空间末尾
  p_space = io_space;
}

//调用paddr_read访问虚拟设备空间，实际上是访问了真实设备空间
//输入物理地址中的虚拟设备空间地址addr，把addr映射到真实设备空间，并返回真实设备空间中的值 + 调用回调函数
word_t map_read(paddr_t addr, int len, IOMap *map) {
  
  assert(len >= 1 && len <= 8);

  //检查addr是否在map范围
  check_bound(map, addr);

  paddr_t offset = addr - map->low;//addr相对于虚拟设备空间起始地址的偏移量

  //调用回调函数
  invoke_callback(map->callback, offset, len, false); // prepare data to read

  word_t ret = host_read(map->space + offset, len);//读取map->space + offset 处 字节为len的内容

  //dtrace:
  printf("io : %s read addr = 0x%x , len = %d , data = 0x%x\n",map->name, addr , len, ret);
  return ret;
}

//输入物理地址(待映射的mmio空间)，写入映射后的设备空间
//space指向的空间就是设备寄存器空间？
void map_write(paddr_t addr, int len, word_t data, IOMap *map) {
  assert(len >= 1 && len <= 8);
  check_bound(map, addr);
  paddr_t offset = addr - map->low;
  host_write(map->space + offset, len, data);
  //调用callback函数
  invoke_callback(map->callback, offset, len, true);
  printf("io : %s write addr = 0x%x , len = %d , data = 0x%x\n",map->name, addr , len, data);
}
