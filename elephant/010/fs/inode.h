#ifndef FS_INODE_H
#define FS_INODE_H
#include "stdint.h"


struct inode {
    uint_32 ino;
    uint_32 i_size;
    uint_32 block[13];
};

#endif
