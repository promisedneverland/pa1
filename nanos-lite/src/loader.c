#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

size_t get_ramdisk_size();
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
static Elf_Ehdr elfHeader;
static Elf_Phdr phdr;

//pcb，filename暂不使用
static uintptr_t loader(PCB *pcb, const char *filename) {
  // printf("init elf : \n");
  // ramdisk_read(&elfHeader,0,sizeof(Elf_Ehdr));

  int fd = fs_open(filename,0,0);
  fs_read(fd,&elfHeader,sizeof(Elf_Ehdr));

  //检查魔数，即是否是elf文件
  assert(elfHeader.e_ident[0] == 0x7f &&
         elfHeader.e_ident[1] == 'E' &&
         elfHeader.e_ident[2] == 'L' &&
         elfHeader.e_ident[3] == 'F');

  // printf("check ok : this is an ELF file\n");

  //检查elf的指定机器结构类型
  #if defined(__ISA_RISCV32__)
    assert(elfHeader.e_machine == EM_RISCV);
    // printf("check ok : this elf has type riscv32\n");
  #endif

  // printf("elfHeader.e_shoff = %d\n",elfHeader.e_shoff);
  for(int i = 0 ; i < elfHeader.e_phnum ; i++)
  {
    fs_lseek(fd,elfHeader.e_phoff + i * elfHeader.e_phentsize,SEEK_SET);
    fs_read(fd,&phdr,elfHeader.e_phentsize);
    // ramdisk_read(&phdr,elfHeader.e_phoff + i * elfHeader.e_phentsize,elfHeader.e_phentsize);
    if(phdr.p_type == PT_LOAD)
    {
      // printf("type load\n");
      // unsigned char* segment_loaded;
      // segment_loaded = malloc(phdr.p_filesz);
      // printf("phdr.p_vaddr = %x\n",phdr.p_vaddr);
      fs_lseek(fd, phdr.p_offset, SEEK_SET);
      fs_read(fd, (char*)phdr.p_vaddr, phdr.p_filesz);
      //  ramdisk_read((char*)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
      // for(int j = 0; j < phdr.p_filesz ; j++)
      //   vaddr_write((uint32_t)1,1,(uint32_t)1);
      // free(segment_loaded);
      memset((char*)(phdr.p_vaddr + phdr.p_filesz),0,phdr.p_memsz - phdr.p_filesz);
    }
    // else 
    // {
    //   printf("\n");
    // }
  }
  // assert(phdr.p_type == RISCV_ATTRIBUT);
  // printf("elfHeader.e_entry; = %x\n",elfHeader.e_entry);
  fs_close(fd);

  return elfHeader.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  // printf("%s","in naive\n\n");
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

