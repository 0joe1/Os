#include "print.h"
#include "init.h"
#include "io.h"
#include "debug.h"
#include "memory.h"
#include "thread.h"

void thread1(void*);

int main(void){
    put_str("kernel starting...\n");
    init();

    thread_start("thread1",31,thread1,"argA ");
    //asm volatile("sti"); 
    while(1);
    return 0;
}
void thread1(void* arg)
{
    while (1){
        put_str((char*)arg);
    }
}
