#ifndef LIB_USR_SYSCALL_H
#define LIB_USR_SYSCALL_H
#include "stdint.h"

enum SYSCALL {
    SYS_GETPID,
};

uint_32 getpid(void);

#endif
