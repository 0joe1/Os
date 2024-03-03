#include "file.h"
#include "memory.h"
#include "string.h"
#include "stdio-kernel.h"
#include "fs.h"
#include "inode.h"
#include "thread.h"
#include "interrupt.h"

struct file file_table[MAX_OPEN_FILES];

int_32 get_free_slot_filetable(void)
{
    for (uint_32 idx = 3 ; idx < MAX_OPEN_FILES ; idx++) {
        if (file_table[idx].inode == NULL)  return idx;
    }
    return -1;
}

int_32 pcb_fd_install(uint_32 fd_idx)
{
    struct task_struct* cur = running_thread();
    for (uint_32 fd = 3 ; fd < MAX_OPEN_FILES_PROC ; fd++)
    {
        if (cur->fd_table[fd] != -1) continue;
        cur->fd_table[fd] = fd_idx;
        return fd;
    }
    return -1;
}

int_32 file_create(struct dir* parent_dir,const char* filename,uint_8 flag)
{
    void* io_buf = sys_malloc(1024);
    if (io_buf == NULL) {
        printk("file create failed: malloc iobuf");
        return -1;
    }

    uint_32 rollback = 0;
    uint_32 ino_idx = inode_bitmap_alloc(cur_part);
    if (ino_idx == -1) {
        printk("inode bitamp alloc failed");
        return -1;
    }

    /* make new inode */
    struct inode* new_inode = malloc_inode();
    if (new_inode == NULL) {
        rollback = 1;
        printk("malloc inode failed\n");
        goto roll_back;
    }
    inode_init(new_inode,ino_idx);

    /* file_table */
    int_32 fidx = get_free_slot_filetable();
    if (fidx == -1) {
        rollback = 2;
        printk("get free slot filetable failed");
        goto roll_back;
    }
    file_table[fidx].inode  = new_inode;
    file_table[fidx].fd_pos = 0;
    file_table[fidx].flag   = flag;

    /* revise parent dir */
    struct dir_entry de;
    memset(&de,0,sizeof(de));
    dir_entry_init(&de,ino_idx,filename,FT_REGULAR);
    if (!sync_dir_entry(parent_dir,&de,io_buf)) {
        rollback = 3;
        printk("sync_dir_entry failed\n");
        goto roll_back;
    }

    /* sync to disk */
    sync_bitmap(cur_part,ino_idx,INODE_BITMAP);
    memset(io_buf,0,1024);
    sync_inode_array(cur_part,parent_dir->inode,io_buf);
    memset(io_buf,0,1024);
    sync_inode_array(cur_part,new_inode,io_buf);

    new_inode->open_cnts = 1;
    list_push(&cur_part->open_inodes,&new_inode->inode_tag);

    sys_free(io_buf);
    return pcb_fd_install(fidx);

roll_back:
    switch(rollback){
        case 3:
            memset(&file_table[fidx],0,sizeof(struct file));
        case 2:
            free_inode(new_inode);
        case 1:
            bitmap_set(&cur_part->inode_bitmap,ino_idx,0);
            sys_free(io_buf);
    }
    return -1;
}

int_32 file_open(uint_32 ino,uint_8 flag)
{
    struct inode* open_inode = inode_open(cur_part,ino);
    Bool* deny_ptr;
    uint_32 fidx = get_free_slot_filetable();
    if (fidx == -1){
        printk("file open: get_free_slot_filetable failed\n");
        return -1;
    }
    file_table[fidx].inode  = open_inode;
    file_table[fidx].fd_pos = 0;
    file_table[fidx].flag   = flag;

    enum intr_status old_status = intr_disable();
    deny_ptr = &open_inode->write_deny;
    if (*deny_ptr == true){
        intr_set_status(old_status);
        printk("can't open when another thread is writing\n");
        return -1;
    }
    *deny_ptr = false;
    intr_set_status(old_status);

    return pcb_fd_install(fidx);
}
