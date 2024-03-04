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

    //process_execute("user_prog_a",u_prog_a);
    //process_execute("user_prog_b",u_prog_b);

    intr_enable();
    //thread_start("k_thread_1",31,k_thread1,"argA ");
    //thread_start("k_thread_2",31,k_thread2,"argB ");

    uint_32 fd = sys_open("/file1",O_CREAT);
    printf("fd=%d\n",fd);
    sys_write(fd,"hello-world\n",12);
    sys_close(fd);
    while(1);
    return 0;
}
void k_thread1(void* arg)
{
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
