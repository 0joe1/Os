#include "syscall.h"
#include "print.h"
#include "thread.h"

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
    put_str("syscall init done\n");
}
