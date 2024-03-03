#include "dir.h"
#include "string.h"
#include "memory.h"
#include "stdio-kernel.h"
#include "fs.h"
#include "debug.h"

void open_root_dir(struct partition* part)
{
    ASSERT(part != NULL);
    root.inode = inode_open(part,part->sb->root_inode_no);
    root.dir_pos = 0;
    memset(root.dir_buf,0,512);
}

struct dir* open_dir(struct partition* part,uint_32 ino)
{
    struct dir* new_dir = sys_malloc(sizeof(struct dir));
    new_dir->inode = inode_open(part,ino);
    new_dir->dir_pos = 0;
    memset(new_dir->dir_buf,0,512);
    return new_dir;
}

void close_dir(struct dir* dir)
{
    if (dir == &root) {
        return ;
    }
    inode_close(dir->inode);
    sys_free(dir);
}

void dir_entry_init(struct dir_entry* de,uint_32 ino,const char* fname,enum filetype type)
{
    de->ino = ino;
    strcpy(de->filename,fname);
    de->ftype = type;
}

Bool sync_dir_entry(struct dir* p_dir,struct dir_entry* de,void* io_buf)
{
    struct super_block* sb = cur_part->sb;
    uint_32* all_blocks = sys_malloc(140*sizeof(uint_32));  //128+12 = 140
    if (all_blocks == NULL) {
        printk("malloc all_blocks failed\n");
        return false;
    }

    struct inode* d_inode = p_dir->inode;
    for (uint_32 blk = 0 ; blk < 12 ; blk++) {
        all_blocks[blk] = d_inode->block[blk];
    }
    if (d_inode->block[12] != 0) {
        ide_read(cur_part->hd,all_blocks+12,d_inode->block[12],1);
    }

    for (uint_32 blk = 0 ; blk < 140 ; blk++)
    {
        if (all_blocks[blk] == 0)
        {
            uint_32 new_sec = block_bitmap_alloc(cur_part);
            if (new_sec == -1) {
                printk("new sec alloc failed\n");
                sys_free(all_blocks);
                return false;
            }
            d_inode->block[blk] = new_sec;

            if (blk == 12)
            {
                uint_32 another_new = block_bitmap_alloc(cur_part);
                if (another_new == -1) {
                    printk("another_new alloc failed\n");
                    d_inode->block[blk] = 0;
                    bitmap_set(&cur_part->block_bitmap,new_sec - sb->data_start_lba,0);
                    sys_free(all_blocks);
                    return false;
                }
                sync_bitmap(cur_part,new_sec - sb->data_start_lba,BLOCK_BITMAP);
                d_inode->block[blk] = another_new;
                sync_bitmap(cur_part,another_new - sb->data_start_lba,BLOCK_BITMAP);
                all_blocks[12] = another_new;
            }
            else
            {
                all_blocks[blk] = new_sec;
            }

            memset(io_buf,0,1024);
            struct dir_entry* de_buf = io_buf;
            memcpy(de_buf,de,sizeof(struct dir_entry));
            ide_write(cur_part->hd,de_buf,all_blocks[blk],1);
            p_dir->inode->i_size += sizeof(struct dir_entry);
            return true;
        }

        ide_read(cur_part->hd,io_buf,all_blocks[blk],1);
        struct dir_entry* de_ptr = io_buf;
        uint_32 max_de_cnt = BYTES_PER_SECTOR/sizeof(struct dir_entry);
        for (uint_32 de_idx = 0 ; de_idx < max_de_cnt ; de_idx++,de_ptr++)
        {
            if (de_ptr->ftype != FT_UNKNOWN) continue;
            memcpy(de_ptr,de,sizeof(struct dir_entry));
            ide_write(cur_part->hd,io_buf,all_blocks[blk],1);
            p_dir->inode->i_size += sizeof(struct dir_entry);
            return true;
        }
    }

    sys_free(all_blocks);
    return false;
}

Bool search_dir_entry(struct partition* part,struct dir* pdir,const char* fname,struct dir_entry* de)
{
    uint_32 blk_cnt = 140;
    uint_32* all_blocks = sys_malloc(140*sizeof(uint_32));
    if (all_blocks == NULL) {
        printk("search_file: all_blocks malloc failed\n");
        sys_free(all_blocks);
        return false;
    }

    struct inode* p_inode = pdir->inode;
    for (uint_32 blk = 0 ; blk < 12 ; blk++) {
        all_blocks[blk] = p_inode->block[blk];
    }
    if (all_blocks[12] != 0) {
        ide_read(cur_part->hd,all_blocks+12,p_inode->block[12],1);
    } else {
        blk_cnt = 12;
    }

    uint_32 max_dir_cnt = BYTES_PER_SECTOR/sizeof(struct dir_entry);
    struct dir_entry* de_buf = sys_malloc(BYTES_PER_SECTOR);
    struct dir_entry* dep = de_buf;
    for (uint_32 blk = 0 ; blk < blk_cnt ; blk++)
    {
        ide_read(part->hd,de_buf,all_blocks[blk],1);
        for (uint_32 d = 0 ; d < max_dir_cnt ; d++,dep++) {
            if (!strcmp(fname,dep->filename)) {
                memcpy(de,dep,sizeof(struct dir_entry));
                sys_free(all_blocks);
                sys_free(de_buf);
                return true;
            }
        }
        dep = de_buf;
        memset(de_buf,0,512);
    }
    sys_free(all_blocks);
    sys_free(de_buf);
    return false;
}

