#include "common.h"
#include "syscall.h"

//uint32_t vaddr_read(vaddr_t addr ,int len);
uintptr_t sys_none(){
    return 1;
}

ssize_t sys_write(int fd,const void *buf,size_t count){
    if (fd==1 || fd == 2){
        int i;
        for (i=0;i<count;i++){
            char c=*((char *)buf);
            _putc(c);
        }
        return i;
    }
    return 0;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
    case SYS_none:{ SYSCALL_ARG1(r)=sys_none();
    break; }

    case SYS_exit:{
        _halt(0);
        break;
    }
    case SYS_write:{
        SYSCALL_ARG1(r)=sys_write(a[0],&a[1],a[2]);
        break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
