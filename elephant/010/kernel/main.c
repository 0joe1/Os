#include "print.h"
#include "init.h"
#include "io.h"
#include "debug.h"
int main(void){
    put_str("kernel starting...\n");
    init();
    ASSERT(1==2);
    asm volatile("sti"); 
    while(1);
    return 0;
}
