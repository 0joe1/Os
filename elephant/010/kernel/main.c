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

    printf("/dir1/subdir1 create %s!\n", \
    sys_mkdir("/dir1/subdir1") == 0 ? "done" : "fail");
    printf("/dir1 create %s!\n", sys_mkdir("/dir1") == 0 ? "done" : "fail");
    printf("now, /dir1/subdir1 create %s!\n", \
    sys_mkdir("/dir1/subdir1") == 0 ? "done" : "fail");
    int fd = sys_open("/dir1/subdir1/file2", O_CREAT|O_RDWT);
    if (fd != -1) {
        printf("/dir1/subdir1/file2 create done!\n");
        sys_write(fd, "Catch me if you can!\n", 21);
        sys_lseek(fd, 0, SEEK_SET);
        char buf[32];
        memset(buf,0,32);
        sys_read(fd, buf, 21);
        printf("/dir1/subdir1/file2 says:\n%s", buf);
        sys_close(fd);
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
