#include "syscall.h"
#include "print.h"
#include "thread.h"
#include "console.h"
#include "string.h"

#define SYSCALL_NR  10
typedef void* syscall;
syscall syscall_table[SYSCALL_NR];

uint_32 sys_getpid(void){
    struct task_struct* cur = running_thread();
    return cur->pid;
}
uint_32 sys_write(char* str)
{
    console_put_str(str);
    return strlen(str);
}

void syscall_init(void)
{
    put_str("syscall init start\n");
    syscall_table[SYS_GETPID] = sys_getpid;
    syscall_table[SYS_WRITE]  = sys_write;
    put_str("syscall init done\n");
}
