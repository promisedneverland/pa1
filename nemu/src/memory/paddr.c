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


//paddr应该从0x80000000,访问0x80000004相当于访问pmem + 4 = pmem[4]
uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }

//haddr应该从pmem[0]开始,pmem[4]的地址为haddr = pmem + 4，对应paddr = 0x80000004
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

//输入paddr,想要读取的字节数，返回4个字节的值
static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

static void out_of_bound(paddr_t addr) {
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, cpu.pc);
}

//默认把所有的pmem内容都设置为随机数
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
    //mtrace:
    //printf("paddr read  addr = 0x%x , len = %d\n", addr , len);
    return pmem_read(addr, len);
  }
  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));
  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  //大概率发生事件
  if (likely(in_pmem(addr))) { 
    //printf("paddr write addr = 0x%x , len = %d , data = 0x%x\n", addr , len, data);
    pmem_write(addr, len, data); 
    return; 
  }
  //小概率发生事件
  
  //dtrace : 
  //printf("io write addr = 0x%x , len = %d , data = 0x%x\n", addr , len, data);

  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);

  //如果执行到这里，则说明没有mmio空间但是又对mmio空间进行了访问
  //打印错误信息
  out_of_bound(addr);
}
