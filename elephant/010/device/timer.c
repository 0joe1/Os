#include "stdint.h"
#include "io.h"
#include "timer.h"
#include "print.h"
#include "interrupt.h"
#include "thread.h"
#include "debug.h"

uint_32 ticks;

static void frequency_set(uint_8 counter_no,uint_8 counter_port, \
                          uint_8 rwl       ,uint_8 work_method,  \
                          uint_16 count_num){
    outb(PIT_CONTROL_PORT,(counter_no<<6)+(rwl<<4)+(work_method<<1));
    outb(counter_port,(uint_8)count_num);
    outb(counter_port,(uint_8)(count_num>>8));
}

void intr_timer_handler(void)
{
    struct task_struct* cur = running_thread();
    ASSERT(cur->kmagic == KMAGIC);
    cur->ticks--;
    cur->elapsed_ticks++;
    ticks++;
    if (cur->ticks == 0){
        schedule();
    }
    return ;
}

void timer_init()
{
    put_str("init timer\n");
    frequency_set(COUNTER0_NO,COUNTER0_PORT,\
                  RW_LH,COUNTER_MODE2,COUNTER0_COUNT_NUM);
    register_handler(0x20,intr_timer_handler);
    put_str("init timer done\n");
}
