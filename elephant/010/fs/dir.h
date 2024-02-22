#ifndef FS_DIR_H
#define FS_DIR_H
#include "stdint.h"

#define FILENAME_MAXLEN 20
enum filetype {
    FT_REGULAR,
    FT_DIRECTORY,
    FT_UNKNOWN
};

struct dir_entry {
    uint_32 ino;
    char filename[FILENAME_MAXLEN];
    enum filetype ftype;
};

#endif
