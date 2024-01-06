#include "print.h"
#include "init.h"
#include "io.h"
int main(void){
    put_str("kernel starting...\n");
    init();
    asm volatile("sti"); 
    while(1);
    return 0;
}
