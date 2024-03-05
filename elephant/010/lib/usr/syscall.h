#ifndef LIB_USR_SYSCALL_H
#define LIB_USR_SYSCALL_H
#include "stdint.h"

enum SYSCALL {
    SYS_GETPID,
    SYS_WRITE,
    SYS_READ,
    SYS_MALLOC,
    SYS_FREE,
};

uint_32 getpid(void);
int_32 write(uint_32 fd,const void* buf,uint_32 count);
int_32 read(uint_32 fd,void* buf,uint_32 count);
void* malloc(uint_32 size);
void free(void* ptr);

#endif
