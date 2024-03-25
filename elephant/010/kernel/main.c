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
#include "fork.h"
#include "shell.h"

//void sss(void)
//{
//    int a =1+1;
//}

void init(void);


void k_thread1(void*);
void k_thread2(void*);
void u_prog_a(void);
void u_prog_b(void);
int prog_a_pid,prog_b_pid;

int main(void){
    put_str("kernel starting...\n");
    init_all();
    intr_enable();
/********************写入应用程序***********************/
    uint_32 filesize = 300;
    struct disk* sda = &channel[0].disk[0];
    void* program = sys_malloc(filesize);
    ASSERT(program != NULL);
    ide_read(sda,program,30,DIV_ROUND_UP(filesize,PAGESIZE));
    int_32 fd = open("try",O_CREAT|O_RDWT);
    ASSERT(fd != -1);
    sys_write(fd,program,filesize);
/*******************************************************/

    cls_screen();
    print_prompt();
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

void init(void)
{
    pid_t retpid = fork();
    if (retpid == 0) {
        my_shell();
    } else {
        printf("I am parent %d , my child is %d\n",getpid(),retpid);
        while(1);
    }
    PANIC("should not be here");
}
