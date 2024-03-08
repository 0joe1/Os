#include "print.h"
#include "init.h"
#include "io.h"
#include "debug.h"
#include "memory.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"
#include "ioqueue.h"
#include "keyboard.h"
#include "process.h"
#include "syscall.h"
#include "syscall-init.h"
#include "stdio.h"
#include "fs.h"
#include "file.h"

void k_thread1(void*);
void k_thread2(void*);
void u_prog_a(void);
void u_prog_b(void);
int prog_a_pid,prog_b_pid;

int main(void){
    put_str("kernel starting...\n");
    init();

    struct dir* p_dir = sys_opendir("/dir1/subdir1");
    if (p_dir) {
        printf("/dir1/subdir1 open done!\n");
        if (sys_closedir(p_dir) == 0) {
            printf("/dir1/subdir1 close done!\n");
        } else {
            printf("/dir1/subdir1 close fail!\n");
        }
    } else {
        printf("/dir1/subdir1 open fail!\n");
    }
    while(1);
    return 0;
}
void k_thread1(void* arg)
{
    char a[20] = {0};
    while(1);
}

void k_thread2(void* arg)
{
    while(1);
}

void u_prog_a(void)
{
    while(1);
}

void u_prog_b(void)
{
    while(1);
}
