#ifndef USRPROG_SYSCALL_INIT_H
#define USRPROG_SYSCALL_INIT_H
#include "stdint.h"

uint_32 sys_getpid(void);
uint_32 write(uint_32 fd,const void* buf,uint_32 count);
void syscall_init(void);

#endif
