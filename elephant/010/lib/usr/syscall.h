#ifndef LIB_USR_SYSCALL_H
#define LIB_USR_SYSCALL_H
#include "stdint.h"

enum SYSCALL {
    SYS_GETPID,
    SYS_WRITE
};

uint_32 getpid(void);
uint_32 write(char*);

#endif
