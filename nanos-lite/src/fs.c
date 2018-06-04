#include "fs.h"

void ramdisk_read(void *buf,off_t offset,size_t len);
typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

ssize_t fs_filesz(int fd){
    return file_table[fd].size;
}
int fs_open(const char *pathname,int flags,int mode){
    int i=0;
    Log("%s",pathname);
    for(i=0;i<NR_FILES;i++){
        if(strcmp(file_table[i].name,pathname)==0){
            file_table[i].open_offset=0;
            return i;

        }
    }
    if (i>=NR_FILES){
        panic("File not found");

    }
    return 0;
}

ssize_t fs_read(int fd,void *buf,size_t len){
    if(fd<=2){
        Log("fd<=2");
        return -1;
    }
    else{
        Finfo temp=file_table[fd];
        Log("%s",temp.name);
        size_t read_size= len < (fs_filesz(fd)-temp.open_offset) ? len : (fs_filesz(fd)-temp.open_offset);
        ramdisk_read(buf,temp.disk_offset+temp.open_offset,read_size);
        file_table[fd].open_offset+=read_size;
        Log("%d",read_size);
        return read_size;
    }
}

int fs_close(int fd){
    return 0;
}
ssize_t fs_write(int fd,const void *buf,size_t count){
    return 0;
}
