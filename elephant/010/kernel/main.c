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

void k_thread1(void*);
void k_thread2(void*);
void u_prog_a(void);
void u_prog_b(void);
int prog_a_pid,prog_b_pid;

int main(void){
    put_str("kernel starting...\n");
    init();

    process_execute("user_prog_a",u_prog_a);
    process_execute("user_prog_b",u_prog_b);

    intr_enable();
    console_put_str(" main_pid:0x");
    console_put_int(sys_getpid());
    console_put_char('\n');
    thread_start("k_thread_1",31,k_thread1,"argA ");
    thread_start("k_thread_2",31,k_thread2,"argB ");
    while(1);
    return 0;
}
void k_thread1(void* arg)
{
    char* para = arg;
    console_put_str(" I am k_thread1, my pid:0x");
    console_put_int(sys_getpid());
    console_put_char('\n');
    while(1);
}

void k_thread2(void* arg)
{
    char* para = arg;
    console_put_str(" I am k_thread2, my pid:0x");
    console_put_int(sys_getpid());
    console_put_char('\n');
    while(1);
}

void u_prog_a(void)
{
    char* name = "prog_a";
    printf(" I am %s,my pid:%d%c",name,getpid(),'\n');
    while(1);
}

void u_prog_b(void)
{
    char* name = "prog_b";
    printf(" I am %s,my pid:%d%c",name,getpid(),'\n');
    while(1);
}
