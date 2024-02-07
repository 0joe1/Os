#include "interrupt.h"
#include "timer.h"
#include "print.h"
#include "init.h"
#include "memory.h"
#include "thread.h"
#include "console.h"
#include "keyboard.h"
#include "global.h" 
#include "tss.h"

void init()
{
    put_str("init all\n");
    idt_init();
    mem_init();
    thread_init();
    timer_init();
    console_init();
    keyboard_init();
    tss_init();
}
