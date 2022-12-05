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
static Elf32_Ehdr elfHeader;
//pcb，filename暂不使用
static uintptr_t loader(PCB *pcb, const char *filename) {
  printf("init elf\n");
  elf = malloc(get_ramdisk_size());
  ramdisk_read(elf,0,get_ramdisk_size());
  ramdisk_read(&elfHeader,0,sizeof(Elf_Ehdr));
  // printf("%s",elf);//将打印ELF
  assert(elfHeader.e_ident[0] == 0x7f &&
         elfHeader.e_ident[1] == 'E' &&
         elfHeader.e_ident[2] == 'L' &&
         elfHeader.e_ident[3] == 'F');

  printf("this is an ELF file\n");


  #if defined(__ISA_RISCV32__)
    assert(elfHeader.e_machine == EM_RISCV);
    printf("this elf has type riscv32\n");
  #elif
  # error unsupported ISA __ISA__
  #endif
  // assert(*(uint32_t *)elf_ehdr-> == 0x7f454c46);

  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  // printf("%s","in naive\n\n");
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  // ((void(*)())entry) ();
}

