#include "fs.h"
#include "global.h"
#include "stdio-kernel.h"
#include "memory.h"
#include "string.h"
#include "dir.h"
#include "debug.h"

void fs_format(struct partition* p)
{
    struct super_block sb;
    sb.magic = LINFSMAGIC;
    sb.block_cnt = p->sec_cnt;
    sb.inode_cnt = MFILES_PER_PARTITION;
    sb.part_lba  = p->start_sec;
    
    uint_32 inode_bitmap_len = MFILES_PER_PARTITION/BITS_PER_SECTOR;
    uint_32 inode_array_len = (sizeof(struct inode)*MFILES_PER_PARTITION)/ \
                              BYTES_PER_SECTOR;
    uint_32 free_len = sb.block_cnt - 2 - inode_array_len - inode_bitmap_len;
    uint_32 max_block_bitmap_len = DIV_ROUND_UP(free_len,BITS_PER_SECTOR);
    uint_32 block_bitmap_len = DIV_ROUND_UP(free_len - max_block_bitmap_len, \
                                            BITS_PER_SECTOR);

    sb.block_bitmap_lba  = p->start_sec + 2;
    sb.block_bitmap_sects = block_bitmap_len;
    sb.inode_bitmap_lba  = sb.block_bitmap_lba + sb.block_bitmap_sects;
    sb.inode_bitmap_sects = inode_bitmap_len;
    sb.inode_array_lba   = sb.inode_bitmap_lba + sb.inode_bitmap_sects;
    sb.inode_array_sects  = inode_array_len;
    sb.data_start_lba    = sb.inode_array_lba + sb.inode_array_sects;
    ide_write(p->hd,&sb,sb.part_lba+1,1);

    printk("    magic:0x%x\n    part_lba_base:0x%x\n    all_sector:0x%x\n" \
           "    inode_cnt:0x%x\n    block_bitmap_lba:0x%x\n"  \
           "    block_bitmap_sectors:0x%x\n    inode_bitmap_lba:0x%x\n" \
           "    inode_bitmap_sectors:0x%x\n    inode_table_lba:0x%x\n" \
           "    inode_table_sectors:0x%x\n    data_start_lba:0x%x\n" \
           "    super_block_lba:0x%x\n    root_dir_lba:0x%x\n", \
           sb.magic,sb.part_lba,sb.block_cnt,sb.inode_cnt,sb.block_bitmap_lba,\
           sb.block_bitmap_sects,sb.inode_bitmap_lba,sb.inode_bitmap_sects, \
           sb.inode_array_lba,sb.inode_array_sects,sb.data_start_lba);

    uint_32 buf_size;
    buf_size = MAX(sb.block_bitmap_sects*512,sb.inode_bitmap_sects*512);
    buf_size = MAX(buf_size,sb.inode_array_sects*512);
    uint_8* buf = sys_malloc(buf_size);

    /* block bitmap */
    buf[0] |= 1;
    uint_32 bit_start = free_len - block_bitmap_len;
    uint_32 bit_end   = sb.block_bitmap_sects * BITS_PER_SECTOR - 1;
    if (bit_start < bit_end)
        for (uint_32 idx = bit_start ; idx <= bit_end ; idx++)
        {
            buf[idx] |= 1;
        }
    ide_write(p->hd,buf,sb.block_bitmap_lba,sb.block_bitmap_sects);

    /* inode bitmap */
    memset(buf,0,buf_size);
    buf[0] |= 1;
    ide_write(p->hd,buf,sb.inode_bitmap_lba,sb.inode_bitmap_sects);

    /* inode array:init root inode */
    memset(buf,0,buf_size);
    struct inode* inode_array = (struct inode*)buf;
    inode_array[0].ino = sb.root_inode_no;
    inode_array[0].i_size = 2*sb.dir_entry_size;
    inode_array[0].block[0] = sb.data_start_lba;
    ide_write(p->hd,buf,sb.inode_array_lba,sb.inode_array_sects);

    /* root block data init */
    memset(buf,0,buf_size);
    struct dir_entry* d_ety = (struct dir_entry*)buf;
    // .
    d_ety->ino = sb.root_inode_no;
    strcpy(d_ety->filename,".");
    d_ety->ftype = FT_DIRECTORY;

    // ..
    d_ety++;
    d_ety->ino = sb.root_inode_no;
    strcpy(d_ety->filename,"..");
    d_ety->ftype = FT_DIRECTORY;
    ide_write(p->hd,buf,sb.data_start_lba,1);
    printk("root_dir_lba:0x%x\n",sb.data_start_lba);

    printk("%s format done\n",p->name);
    sys_free(buf);
}


void fs_init(void)
{
    struct super_block* sb = sys_malloc(512);
    if (sb == NULL) {
        PANIC("fs init malloc sb failed");
    }
    printk("searching filesystem...\n");
    for (uint_32 dev_no = 1 ; dev_no < 4 ; dev_no++)
    {
        struct disk* hd = &channel[dev_no/2].disk[dev_no%2];
        struct partition* p = hd->primary;
        for (uint_32 par_no = 0 ; par_no < 11 ; par_no++)
        {
            if (par_no == 4) {
                p = hd->logic;
            }
            if (p->sec_cnt == 0) {
                p++;
                continue;
            }

            ide_read(hd,sb,p->start_sec+1,1);
            if (sb->magic == LINFSMAGIC) {
                printk("%s has filesystem\n",p->name);
            } else {
                printk("formatting %s's partition %s\n",hd->name,p->name);
                fs_format(p);
            }

            memset(sb,0,512);
            p++;
        }
    }
    sys_free(sb);
}
