#include "interrupt.h"
#include "timer.h"
#include "print.h"
#include "init.h"
#include "memory.h"
void init()
{
    put_str("init all\n");
    idt_init();
    mem_init();
    timer_init();
}
