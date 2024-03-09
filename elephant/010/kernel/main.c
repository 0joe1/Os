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

    struct stat obj_stat;
    sys_stat("/", &obj_stat);
    printf("/`s info\ni_no:%d\nsize:%d\nfiletype:%s\n", \
           obj_stat.st_ino, obj_stat.st_fsize, \
           obj_stat.st_ftype == 2 ? "directory" : "regular");
    sys_stat("/dir1", &obj_stat);
    printf("/dir1`s info\ni_no:%d\nsize:%d\nfiletype:%s\n", \
           obj_stat.st_ino, obj_stat.st_fsize, \
           obj_stat.st_ftype == 2 ? "directory" : "regular");

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
