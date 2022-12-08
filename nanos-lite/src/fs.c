#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
//文件记录表，其下标为文件描述符
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

static int file_num; //文件数量
void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_num = sizeof(file_table) / sizeof(Finfo);
}

int fs_open(const char *path, int flags, unsigned int mode)
{
  for(int i = 0 ; i < file_num; i++)
  {
    if(strcmp( path, file_table[i].name) == 0)
    {
      return i;
    }
  }
  printf("file %s not found\n",path);
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  //文件偏移量超过了文件大小
  if(file_table[fd].open_offset >= file_table[fd].size)
  {
    return 0;
  }

  //文件偏移量加上读取的字节数 超过了文件大小
  if(file_table[fd].open_offset + len > file_table[fd].size)
  {
    file_table[fd].open_offset = file_table[fd].size;
    return ramdisk_read(buf,file_table[fd].disk_offset,file_table[fd].size - file_table[fd].open_offset);
  }
  else
  {
    file_table[fd].open_offset += len;
    return ramdisk_read(buf,file_table[fd].disk_offset,len);
  }
}
size_t fs_write(int fd, const void *buf, size_t len)
{
  //文件偏移量超过了文件大小
  if(file_table[fd].open_offset >= file_table[fd].size)
  {
    return 0;
  }

  //文件偏移量加上读取的字节数 超过了文件大小
  if(file_table[fd].open_offset + len > file_table[fd].size)
  {
    file_table[fd].open_offset = file_table[fd].size;
    return ramdisk_write(buf,file_table[fd].disk_offset,file_table[fd].size - file_table[fd].open_offset);
  }
  else
  {
    file_table[fd].open_offset += len;
    return ramdisk_write(buf,file_table[fd].disk_offset,len);
  }
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  //根据whence设置open_offset
  if(whence == SEEK_SET)
  {
    file_table[fd].open_offset = offset;
  }
  else if(whence == SEEK_CUR)
  {
    file_table[fd].open_offset += offset;
  }
  else if(whence == SEEK_END)
  {
    file_table[fd].open_offset = offset + file_table[fd].size;
  }
  return file_table[fd].open_offset;
}
int fs_close(int fd)
{
 return 0; 
}
