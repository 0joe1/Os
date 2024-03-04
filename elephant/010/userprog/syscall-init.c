#include "syscall.h"
#include "print.h"
#include "thread.h"
#include "console.h"
#include "string.h"
#include "memory.h"
#include "fs.h"

#define SYSCALL_NR  10
typedef void* syscall;
syscall syscall_table[SYSCALL_NR];

uint_32 sys_getpid(void){
    struct task_struct* cur = running_thread();
    return cur->pid;
}

void syscall_init(void)
{
    put_str("syscall init start\n");
    syscall_table[SYS_GETPID] = sys_getpid;
    syscall_table[SYS_WRITE]  = sys_write;
    syscall_table[SYS_MALLOC] = sys_malloc;
    syscall_table[SYS_FREE]   = sys_free;
    put_str("syscall init done\n");
}
