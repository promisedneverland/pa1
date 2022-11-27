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

#include <memory/host.h>
#include <memory/paddr.h>
#include <device/mmio.h>
#include <isa.h>

//创建pmem数组，大小为0x80000000
//PG_ALIGN不知道什么意思
#if   defined(CONFIG_PMEM_MALLOC)
static uint8_t *pmem = NULL;
#else // CONFIG_PMEM_ARRAY
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};//CONFIG_MSIZE = 128MB,pmem作为内存
#endif

//猜测：paddr是+8000000的，v不加
uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
//初始时，返回指针指向pmem[0]
//返回pmem数组下标处
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);//读取pmem值，paddr是+80000000的值，len是字节数
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

static void out_of_bound(paddr_t addr) {
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
}

//默认其实什么都没做
void init_mem() {
#if   defined(CONFIG_PMEM_MALLOC)//如果定义了 则执行
  pmem = malloc(CONFIG_MSIZE);
  assert(pmem);
#endif
#ifdef CONFIG_MEM_RANDOM//是一个设置，如果被定义了，则把所有的pmem内容都设置为随机数
  uint32_t *p = (uint32_t *)pmem;//p指向pmem[0]，32位
  int i;
  for (i = 0; i < (int) (CONFIG_MSIZE / sizeof(p[0])); i ++) {
    p[i] = rand();
  }
#endif
  //目前执行下面一行"0x%08x"
  // Log("physical memory area [" "0x%08x" ", " "0x%08x" "]", 80000000, FFFFFFFF);
  //用printf的格式，但是有特殊功能：能输出到log文件，设置了颜色
  Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]", PMEM_LEFT, PMEM_RIGHT);
}

word_t paddr_read(paddr_t addr, int len) {
  if (likely(in_pmem(addr))) 
  {
    //printf("paddr read  addr = 0x%x , len = %d\n", addr , len);
    return pmem_read(addr, len);
  }
  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));
  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  if (likely(in_pmem(addr))) { 
    //printf("paddr write addr = 0x%x , len = %d , data = 0x%x\n", addr , len, data);
    pmem_write(addr, len, data); 
    return; 
  }
  //printf("io write addr = 0x%x , len = %d , data = 0x%x\n", addr , len, data);
  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  //如果执行到这里，则说明越界
  out_of_bound(addr);
}
