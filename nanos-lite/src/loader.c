#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
extern void ramdisk_read(void *buf,off_t offset,size_t len);
extern size_t get_ramdisk_size();
int fs_open(const char *pathname,int flags,int mode);
ssize_t fs_read(int fd,void *buf,size_t len);
int fs_close(int fd);
ssize_t fs_filesz(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
 // ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
    Log("loader filename %s",filename);
   int fd=fs_open(filename,0,0);
   ssize_t a=fs_read(fd,DEFAULT_ENTRY,fs_filesz(fd));
   int b=fs_close(fd);
   if(a!=0&&b==0)
  return (uintptr_t)DEFAULT_ENTRY;
  else
  return (uintptr_t)DEFAULT_ENTRY;
}
