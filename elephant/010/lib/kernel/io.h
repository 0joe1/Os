#include "stdint.h"
#ifndef IO_H
#define IO_H

static inline void outb(uint_16 port,uint_8 data){
    asm volatile ("outb %%al,%%dx"::"a"(data),"d"(port));
}

static inline void outsw(uint_16 port,void* buf,uint_32 wcnt){
    asm volatile ("outsw %%dx,%%edi":"+D"(buf):"d"(port),"a"(wcnt):"memory");
}

static inline uint_8 inb(uint_16 port) {
    uint_8 data;
    asm volatile ("inb %%dx,%b0":"=a"(data):"d"(port));
    return data;
}

static inline void insw(uint_16 port,void* buf,uint_32 wcnt) {
    asm volatile ("insw %%dx,%%esi":"+S"(buf):"d"(port),"a"(wcnt):"memory");
}

#endif
