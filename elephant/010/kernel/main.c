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
    intr_enable();

    uint_32 fd = sys_open("/file1",O_RDWT);
    printf("open /file1,fd=%d\n",fd);
    char buf[64];
    memset(buf,0,64);
    int read_bytes = sys_read(fd,buf,18);
    printf("1_ read %d bytes:\n%s\n",read_bytes,buf);

    memset(buf,0,64);
    read_bytes = sys_read(fd,buf,6);
    printf("2_ read %d bytes:\n%s\n",read_bytes,buf);

    memset(buf,0,64);
    read_bytes = sys_read(fd,buf,6);
    printf("3_ read %d bytes:\n%s\n",read_bytes,buf);

    printf("______________ SEEK SET 0 ____________\n");
    sys_lseek(fd,0,SEEK_SET);
    memset(buf,0,64);
    read_bytes = sys_read(fd,buf,24);
    printf("4_ read %d bytes:\n%s",read_bytes,buf);

    sys_close(fd);
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
