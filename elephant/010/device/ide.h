#ifndef DEVICE_IDE_H
#define DEVICE_IDE_H
#include "stdint.h"
#include "sync.h"

struct disk {
    char name[17];
    struct channel* chnl;
    uint_32 dev_no;
};

struct channel {
    uint_32 port_base;
    uint_32 int_no;
    Bool expecting_intr;
    struct lock lock;
    struct semaphore wait;  //不能放在disk里，若放disk，intr_hd_handler不知道是哪个disk
    struct disk disk[2];
};

static Bool busy_wait(struct disk* hd); 
void ide_init(void);
void select_disk(struct disk* hd);
void select_sector(struct disk* hd,uint_32 start_lba,uint_8 sec_cnt);
void cmd_out(struct disk* hd,uint_8 cmd);
void identify_disk(struct disk* hd);
void swap_pair_bytes(const char* src,char* dst,uint_32 len);
void intr_hd_handler(uint_32 int_num);

#endif
