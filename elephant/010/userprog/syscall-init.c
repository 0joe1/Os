#include "syscall.h"
#include "print.h"
#include "thread.h"
#include "console.h"
#include "string.h"
#include "memory.h"
#include "fs.h"
#include "fork.h"
#include "print.h"

#define SYSCALL_NR  30
typedef void* syscall;
syscall syscall_table[SYSCALL_NR];


uint_32 sys_getpid(void){
    struct task_struct* cur = running_thread();
    return cur->pid;
}

void sys_putchar(char c) {
    console_put_char(c);
}

void syscall_init(void)
{
    put_str("syscall init start\n");
    syscall_table[SYS_GETPID]   = sys_getpid;
    syscall_table[SYS_WRITE]    = sys_write;
    syscall_table[SYS_MALLOC]   = sys_malloc;
    syscall_table[SYS_FREE]     = sys_free;
    syscall_table[SYS_READ]     = sys_read;
    syscall_table[SYS_FORK]     = sys_fork;
    syscall_table[SYS_PUTCHAR]  = sys_putchar;
    syscall_table[SYS_CLS]      = cls_screen;
    syscall_table[SYS_GETCWD]   = sys_getpid;
    syscall_table[SYS_OPEN]     = sys_open;
    syscall_table[SYS_CLOSE]    = sys_close;
    syscall_table[SYS_LSEEK]    = sys_lseek;
    syscall_table[SYS_UNLINK]   = sys_unlink;
    syscall_table[SYS_MKDIR]    = sys_mkdir;
    syscall_table[SYS_OPENDIR]  = sys_opendir;
    syscall_table[SYS_CLOSEDIR] = sys_closedir;
    syscall_table[SYS_CHDIR]    = sys_chdir;
    syscall_table[SYS_RMDIR]    = sys_rmdir;
    syscall_table[SYS_READDIR]  = sys_readdir;
    syscall_table[SYS_REWINDDIR]= sys_rewinddir;
    syscall_table[SYS_STAT]     = sys_stat;
    syscall_table[SYS_PS]       = sys_ps;

    put_str("syscall init done\n");
}
