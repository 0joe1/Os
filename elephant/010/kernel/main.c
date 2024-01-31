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

void thread1(void*);
void thread2(void*);

int main(void){
    put_str("kernel starting...\n");
    init();

    thread_start("thread1",20,thread1,"argA_ ");
    thread_start("thread2",20,thread2,"argB_ ");
    intr_enable();
    while(1){
        //console_put_str("main ");
    }
    return 0;
}
void thread1(void* arg)
{
    while (1){
        enum intr_status old_status = intr_disable();
        char byte = ioq_getchar(&kbd_buf);
        console_put_char(byte);
        intr_set_status(old_status);
    }
}

void thread2(void* arg)
{
    while (1){
        enum intr_status old_status = intr_disable();
        char byte = ioq_getchar(&kbd_buf);
        console_put_char(byte);
        intr_set_status(old_status);
    }
}
