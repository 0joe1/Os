#include "stdint.h"
#include "io.h"
#include "timer.h"
#include "print.h"

static void frequency_set(uint_8 counter_no,uint_8 counter_port, \
                          uint_8 rwl       ,uint_8 work_method,  \
                          uint_16 count_num){
    outb(PIT_CONTROL_PORT,(counter_no<<6)+(rwl<<4)+(work_method<<1));
    outb(counter_port,(uint_8)count_num);
    outb(counter_port,(uint_8)(count_num>>8));
}

void timer_init()
{
    put_str("init timer");
    frequency_set(COUNTER0_NO,COUNTER0_PORT,\
                  RW_LH,COUNTER_MODE2,COUNTER0_COUNT_NUM);
    put_str("init timer done");
}
