#include "interrupt.h"
#include "timer.h"
#include "print.h"
#include "init.h"
void init()
{
    put_str("init all\n");
    idt_init();
    timer_init();
}
