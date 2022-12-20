#include <fs.h>

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fbctr_write(const void *buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_FBCTR, FD_FB, FD_DISIN, FD_END };

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
  [FD_STDIN]  = {"stdin",        0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout",       0, 0, invalid_read, serial_write },
  [FD_STDERR] = {"stderr",       0, 0, invalid_read, invalid_write},
  [FD_EVENT]  = {"/dev/events",  0, 0, events_read, invalid_write},
  [FD_FBCTR]  = {"/dev/fbctr" ,  0, 0, invalid_read, fbctr_write},
  [FD_FB]     = {"/dev/fb"    ,  0, 0, invalid_read, fb_write},
  [FD_DISIN]  = {"/proc/dispinfo",0, 0, dispinfo_read, fb_write},
  [FD_END]    = {"nothing",      0, 0, invalid_read, invalid_write},
#include "files.h"
};

static int file_num; //文件数量
void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_num = sizeof(file_table) / sizeof(Finfo);
  AM_GPU_CONFIG_T gc = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = gc.height * gc.width + 5;
  printf("%d\n",gc.height);
}

int fs_open(const char *path, int flags, unsigned int mode)
{
  //对文件名进行比对
  for(int i = 0 ; i < file_num; i++)
  {
    if(strcmp( path, file_table[i].name) == 0)
    {
      return i;
    }
  }

  //未找到匹配的文件
  printf("file %s not found\n",path);
  assert(0);
  return -1;
}

int fs_read(int fd, void *buf, int len)
{
  if(fd <= FD_END)
  {
    return file_table[fd].read(buf,0,len);
  }

  int ret = 0;
  //文件偏移量超过了文件大小
  // printf("fs_read fd = %d , open_offset = %x, len = %d\n",fd,file_table[fd].open_offset,len);
  if(file_table[fd].open_offset >= file_table[fd].size)
  {
    ret = 0;
  }

  //文件偏移量加上读取的字节数 超过了文件大小
  if(file_table[fd].open_offset + len > file_table[fd].size)
  {
    ret = ramdisk_read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, file_table[fd].size - file_table[fd].open_offset);
    file_table[fd].open_offset = file_table[fd].size;
  }
  else
  {
    ret = ramdisk_read(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
  }
  return ret;
}
size_t fs_write(int fd, const void *buf, size_t len)
{
  //printf 写入标准输入输出
  // if(fd == 1 || fd == 2)
  // {
  //   for(int i = 0 ; i < len; i++)
  //   {
  //     putch(*((char*)buf + i));
  //   }
  //   return len;
  // }
  
  if(fd <= FD_END)
    return file_table[fd].write(buf,0,len);
  
  int ret = 0;
  // printf("fs_write fd = %d , open_offset = %x, len = %d\n",fd,file_table[fd].open_offset,len);
  //文件偏移量超过了文件大小
  if(file_table[fd].open_offset >= file_table[fd].size)
  {
    ret = 0;
  }

  //文件偏移量加上读取的字节数 超过了文件大小
  if(file_table[fd].open_offset + len > file_table[fd].size)
  {
    ret = ramdisk_write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, file_table[fd].size - file_table[fd].open_offset);
    file_table[fd].open_offset = file_table[fd].size;
  }
  else
  {
    ret = ramdisk_write(buf, file_table[fd].open_offset + file_table[fd].disk_offset, len);
    file_table[fd].open_offset += len;
  }
  return ret;
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
