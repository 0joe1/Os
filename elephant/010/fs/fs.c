#include "fs.h"
#include "thread.h"
#include "global.h"
#include "stdio-kernel.h"
#include "memory.h"
#include "string.h"
#include "dir.h"
#include "debug.h"
#include "list.h"
#include "file.h"

struct partition* cur_part;
struct dir root;

void fs_format(struct partition* p)
{
    struct super_block sb;
    sb.magic = LINFSMAGIC;
    sb.dir_entry_size = sizeof(struct dir_entry);
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
           "    inode_table_sectors:0x%x\n    data_start_lba:0x%x\n" ,\
           sb.magic,sb.part_lba,sb.block_cnt,sb.inode_cnt,sb.block_bitmap_lba,\
           sb.block_bitmap_sects,sb.inode_bitmap_lba,sb.inode_bitmap_sects, \
           sb.inode_array_lba,sb.inode_array_sects,sb.data_start_lba);

    uint_32 buf_size;
    buf_size = MAX(sb.block_bitmap_sects*512,sb.inode_bitmap_sects*512);
    buf_size = MAX(buf_size,sb.inode_array_sects*512);
    uint_8* buf = sys_malloc(buf_size);

    /* block bitmap */
    buf[0] |= 1;
    uint_32 bit_start = free_len - max_block_bitmap_len;
    uint_32 bit_end   = sb.block_bitmap_sects * BITS_PER_SECTOR - 1;
    if (bit_start < bit_end)
        for (uint_32 idx = bit_start ; idx <= bit_end ; idx++)
        {
            buf[idx/8] |= (1<<(idx%8));
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

    char default_part[8] = "sdb1";
    list_traversal(&partition_list,mount_partition,(int)default_part);

    open_root_dir(cur_part);
    for (uint_32 fd_idx = 0 ; fd_idx < MAX_OPEN_FILES ; fd_idx++){
        file_table[fd_idx].inode = NULL;
    }
}

Bool mount_partition(struct list_elm* pt_elm,int arg)
{
    char* part_name = (char*)arg;
    struct partition* pt = mem2entry(struct partition,pt_elm,part_elm);
    if (strcmp(pt->name,part_name)) {
        printk("false:%s\n",pt->name);
        return false;
    }

    cur_part = pt;
    cur_part->sb = sys_malloc(BYTES_PER_SECTOR);    
    if (cur_part->sb == NULL) {
        PANIC("mount_partition alloc memory failed");
    }
    ide_read(pt->hd,cur_part->sb,pt->start_sec+1,1);
  
    struct super_block* sb = cur_part->sb;
    /* block bitmap */
    cur_part->block_bitmap.map_size = sb->block_bitmap_sects*BYTES_PER_SECTOR;
    cur_part->block_bitmap.bits = sys_malloc(cur_part->block_bitmap.map_size);
    if (cur_part->block_bitmap.bits == NULL) {
        PANIC("mount_partition alloc memory failed");
    }
    ide_read(pt->hd,cur_part->block_bitmap.bits,sb->block_bitmap_lba,sb->block_bitmap_sects);

    /* inode */
    cur_part->inode_bitmap.map_size = sb->inode_bitmap_sects*BYTES_PER_SECTOR;
    cur_part->inode_bitmap.bits = sys_malloc(cur_part->inode_bitmap.map_size);
    if (cur_part->inode_bitmap.bits == NULL) {
        PANIC("mount_partition alloc memory failed");
    }
    ide_read(pt->hd,cur_part->inode_bitmap.bits,sb->inode_bitmap_lba,sb->inode_bitmap_sects);
    list_init(&cur_part->open_inodes);

    printk("mount %s done!\n",cur_part->name);
    return true;
}

int_32 inode_bitmap_alloc(struct partition* part)
{
    struct bitmap* inode_btmp = &part->inode_bitmap;
    uint_32 free_pos = bit_scan(inode_btmp,1);
    if (free_pos == -1) {
        return -1;
    }
    bitmap_set(inode_btmp,free_pos,1);
    return free_pos;
}

int_32 block_bitmap_alloc(struct partition* part)
{
    struct bitmap* block_btmp = &part->block_bitmap;
    struct super_block* sb = part->sb;
    uint_32 sec_off = bit_scan(block_btmp,1);
    if (sec_off == -1) {
        return -1;
    }
    bitmap_set(block_btmp,sec_off,1);
    return sb->data_start_lba + sec_off;
}

void sync_bitmap(struct partition* part,uint_32 bit_idx,uint_8 type)
{
    ASSERT(type==INODE_BITMAP || type==BLOCK_BITMAP);
    struct super_block* sb = part->sb;
    uint_32 lba_off    = bit_idx/8/BYTES_PER_SECTOR;
    uint_32 start_byte = lba_off*BYTES_PER_SECTOR;

    struct bitmap* btmp;
    switch(type){
        case INODE_BITMAP:
            btmp = &part->inode_bitmap;
            ide_write(part->hd,btmp->bits+start_byte,sb->inode_bitmap_lba+lba_off,1);
            break;
        case BLOCK_BITMAP:
            btmp = &part->block_bitmap;
            ide_write(part->hd,btmp->bits+start_byte,sb->block_bitmap_lba+lba_off,1);
            break;
    }
}

const char* path_parse(const char* path,char* name)
{
    const char* pathp = path;
    while (pathp[0] == '/') pathp++;
    while (*pathp && *pathp != '/') (*name++ = *pathp++);
    *name = '\0';
    if (*pathp || *(pathp+1)){
        return NULL;
    }
    return pathp;
}

uint_32 path_depth_cnt(const char* path)
{
    ASSERT(path != NULL);
    uint_32 counter = 0;
    char buf[FILENAME_MAXLEN];
    const char* subpath = path;
    while ((subpath = path_parse(subpath,buf))) {
        counter++;
        memset(buf,0,sizeof(buf));
    }
    return counter;
}

int_32 search_file(const char* filename,struct path_search_record* record)
{
    const char* subpath = filename;
    char cur_name[FILENAME_MAXLEN];
    struct dir_entry de;

    record->p_dir = &root;
    uint_32 grandfather = record->p_dir->inode->ino;
    subpath = path_parse(subpath,cur_name);
    while (*subpath)
    {
        strcat(record->searched_path,"/");
        strcat(record->searched_path,cur_name);
        if (!search_dir_entry(cur_part,record->p_dir,cur_name,&de)) {
            printk("search_dir_entry failed\n");
            printk("searched path:%s\n",record->searched_path);
            return -1;
        }
        record->ftype = de.ftype;
        if (de.ftype == FT_DIRECTORY)
        {
            grandfather = record->p_dir->inode->ino;
            close_dir(record->p_dir);
            record->p_dir = open_dir(cur_part,de.ino);
            subpath = path_parse(subpath,cur_name);
            continue;
        }
        else if(de.ftype == FT_REGULAR)
        {
            return de.ino;
        }
        else
        {
            return -1;
        }
    }

    close_dir(record->p_dir);
    record->p_dir = open_dir(cur_part,grandfather);
    return de.ino;
}

int_32 sys_open(const char* filename,uint_8 flag)
{
    if (filename[strlen(filename)-1] == '/') {
        printk("use dir_open to open directory\n");
        return -1;
    }
    struct path_search_record record;
    memset(&record,0,sizeof(record));
    int_32 ino = search_file(filename,&record);
    Bool exist = ino==-1 ? 0 : 1;
    /* remove anormalies */
    if (!exist && !(flag&O_CREAT)) {
        printk("the file you search doesn't exists\n");
        printk("searched path:%s\n",record.searched_path);
        return -1;
    }
    if (exist && record.ftype==FT_DIRECTORY) {
        printk("use dir_open to open directory\n");
        return -1;
    }

    int_32 fd;
    if (exist)
    {
        printk("file exists\n");
        fd = file_open(ino,flag);
    }
    else
    {
        printk("creating new file...\n");
        const char* new_fname = strrchr(filename,'/')+1;
        fd = file_create(record.p_dir,new_fname,flag);
    }
    return fd;
}

int_32 fdlocal2gloabl(int_32 local_fd)
{
    struct task_struct* cur = running_thread();
    int_32 _fd = cur->fd_table[local_fd];
    return _fd;
}

int_32 sys_close(int_32 fd)
{
    int_32 _fd = fdlocal2gloabl(fd);
    free_inode(file_table[_fd].inode);
    file_table[_fd].inode = NULL;
    struct task_struct* cur = running_thread();
    cur->fd_table[fd] = -1;
    printk("fd %d closed\n",fd);

    return 0;
}

