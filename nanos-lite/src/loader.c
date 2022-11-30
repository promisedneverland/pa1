#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif
size_t get_ramdisk_size();
//pcb，filename暂不使用
static uintptr_t loader(PCB *pcb, const char *filename) {
  printf("%d\n\n",get_ramdisk_size());
  
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  // printf("%s","in naive\n\n");
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

