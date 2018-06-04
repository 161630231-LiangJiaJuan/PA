#include "common.h"
#include "syscall.h"

//uint32_t vaddr_read(vaddr_t addr ,int len);

extern char  _end;

uintptr_t sys_none(){
    return 1;
}

ssize_t sys_write(int fd,const void *buf,size_t count){
    if (fd==1 || fd == 2){
        int i;
        for (i=0;i<count;i++){
            char c=*((char *)(buf+i));
           // Log("%c",c);
            _putc(c);
        }
        return i;
    }
    return 0;
}

int  sys_brk(unsigned long addr){
    _end+=addr;
       // Log("sys_brk %x %x",_end,addr);
    return 0;
}

int fs_open(const char *path,int flags,int mode);
int sys_open(const char *path,int flags,mode_t mode){
    Log("open file %s",path);
    return fs_open(path,flags,mode);
}

int fs_read(int fd,void *buf,size_t len);
ssize_t sys_read(int fd,void *buf,size_t len){
    
    return fs_read(fd,buf,len);
}

int fs_close(int fd);
int sys_close(unsigned int fd){
    return fs_close(fd);
}

ssize_t fs_write(int fd,const void* buf,size_t len);

off_t fs_lseek(int fd, off_t offset,int whence);
off_t sys_lseek(int fd,off_t offset,int whence){
    return fs_lseek(fd,offset,whence);
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  switch (a[0]) {
    case SYS_none:{ 
       // Log("sys_none");
        SYSCALL_ARG1(r)=sys_none();
    break; }

    case SYS_exit:{
        //Log("sys_exit");
        _halt(0);
        break;
    }
    case SYS_write:{
        //Log("sys_write");
        SYSCALL_ARG1(r)=fs_write(r->ebx,(void *)r->ecx,r->edx);
        //SYSCALL_ARG1(r)=sys_write(r->ebx,(void *)r->ecx,r->edx);
        break;
    }
    case SYS_brk:{
        SYSCALL_ARG1(r)=sys_brk((unsigned long)r->ebx);
        //Log("sys_brk %d ",r->eax);
        break;
    }
    case SYS_open:{
        SYSCALL_ARG1(r)=sys_open((const char*)r->ebx,r->ecx,r->edx);
       // Log("sys_open");
        break;
    }
    case SYS_read:{
        SYSCALL_ARG1(r)=sys_read(r->ebx,(void *)r->ecx,r->edx);
       // Log("sys_read");
        break;
    }
    case SYS_close:{
        SYSCALL_ARG1(r)=sys_close((unsigned int)r->ebx);
        break;
    }
    case SYS_lseek:{
        SYSCALL_ARG1(r)=sys_lseek(r->ebx,r->ecx,r->edx);
        break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
