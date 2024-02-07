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

void k_thread1(void*);
void k_thread2(void*);
void u_prog_a(void);
void u_prog_b(void);
int test_var_a=0,test_var_b=0;

int main(void){
    put_str("kernel starting...\n");
    init();

    thread_start("k_thread1",20,k_thread1,"argA_ ");
    thread_start("k_thread2",20,k_thread2,"argB_ ");
    process_execute("user_prog_a",u_prog_a);
    process_execute("user_prog_b",u_prog_b);
    intr_enable();
    while(1){
        //console_put_str("main ");
    }
    return 0;
}
void k_thread1(void* arg)
{
    while (1){
        console_put_str("v_a:0x");
        console_put_int(test_var_a);
    }
}

void k_thread2(void* arg)
{
    while (1){
        console_put_str("v_b:0x");
        console_put_int(test_var_b);
    }
}

void u_prog_a(void)
{
    while(1){
        test_var_a++;
    }
}

void u_prog_b(void)
{
    while(1){
        test_var_b++;
    }
}
