#ifndef KERNEL_GLOBAL_H
#define KERNEL_GLOBAL_H

#define DESC_P 1      //c里面没有用后面加个b表示二进制的方法
#define DPL_0  0
#define DPL_3  3

/***********************selector*************************/
#define TI_GDT  0
#define TI_LDT  4
#define SELECTOR_CODE (0x01<<3) + TI_GDT + DPL_0

/**********************descriptor************************/
#define INT_DESC_TYPE 0xe

#define INT_ATTR_DPL0 ((DESC_P<<7)+(DPL_0<<5)+INT_DESC_TYPE)
#define INT_ATTR_DPL3 ((DESC_P<<7)+(DPL_3<<5)+INT_DESC_TYPE)

/**********************other************************/

#endif
