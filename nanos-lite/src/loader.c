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
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
char* elf;
//pcb，filename暂不使用
static uintptr_t loader(PCB *pcb, const char *filename) {
  printf("init elf\n\n");
  ramdisk_read(elf,0,get_ramdisk_size());
  printf("\n\n");
  // printf("%s",elf);
  // assert(*(uint32_t *)elf->e_ident == 0xBadC0de);

  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  // printf("%s","in naive\n\n");
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  // ((void(*)())entry) ();
}

