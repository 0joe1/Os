#ifndef FS_H
#define FS_H
#include "ide.h"
#include "super_block.h"
#include "inode.h"

#define LINFSMAGIC 0x27308192
#define MFILES_PER_PARTITION 4096
#define BYTES_PER_SECTOR 512
#define BITS_PER_SECTOR (BYTES_PER_SECTOR*8)

extern struct channel channel[];
extern struct list partition_list;

void fs_format(struct partition*);
void fs_init(void);
Bool mount_partition(struct list_elm* pt_elm,int arg);

#endif
