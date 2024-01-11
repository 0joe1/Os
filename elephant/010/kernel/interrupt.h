#ifndef INTERRUPT_H
#define INTERRUPT_H
#include "stdint.h"

#define IDT_DESC_NUMBER 0x21

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1

typedef void* gate_addr;
typedef struct {
    uint_16 addr_low16;
    uint_16 selector;
    uint_8  no_use;
    uint_8  attribute;
    uint_16 addr_high_16;
}int_gate_desc;

static int_gate_desc gate_desc_table[IDT_DESC_NUMBER];

void make_idt_desc(int_gate_desc* ,uint_8 ,gate_addr );
void init_pic(void);
void init_idt_desc(void);
void idt_init(void);


#endif
