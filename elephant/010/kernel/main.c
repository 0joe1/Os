#include "print.h"
#include "init.h"
#include "io.h"
#include "debug.h"
#include "memory.h"
int main(void){
    put_str("kernel starting...\n");
    init();
    void* addr = get_kernel_pages(3);
    put_str("\n get kernel page start vaddr is ");
    put_int((uint_32)addr);
    put_str("\n");
    //asm volatile("sti"); 
    while(1);
    return 0;
}
