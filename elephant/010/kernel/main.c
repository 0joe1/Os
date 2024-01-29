#include "print.h"
#include "init.h"
#include "io.h"
#include "debug.h"
#include "memory.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"

void thread1(void*);
void thread2(void*);

int main(void){
    put_str("kernel starting...\n");
    init();

    //thread_start("thread1",31,thread1,"argA ");
    //thread_start("thread2",8,thread1,"argB ");
    intr_enable();
    while(1){
        //console_put_str("main ");
    }
    return 0;
}
void thread1(void* arg)
{
    while (1){
        console_put_str((char*)arg);
    }
}

void thread2(void* arg)
{
    while (1){
        console_put_str((char*)arg);
    }
}
