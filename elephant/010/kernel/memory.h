#ifndef KERNEL_MEMORY_H
#define KERNEL_MEMORY_H
#include "bitmap.h"

struct virt_addr {
    struct bitmap btmp;
    uint_32 vaddr_start;
};

struct pool {
    struct bitmap btmp;
    uint_32 paddr_start;
    uint_32 pool_size;
};

enum pool_flag {
    PF_KERNEL,
    PF_USER
};


void mem_pool_init(uint_32 total_mem);
void mem_init(void);
static void* get_vaddr(enum pool_flag,uint_32 pcnt);
uint_32* pte_ptr(uint_32 vaddr);
uint_32* pde_ptr(uint_32 vaddr);
static void page_table_add(void* _vaddr,void* _paddr);
void* malloc_page(enum pool_flag pf,uint_32 pcnt);
void* get_kernel_pages(uint_32 pcnt);

#endif
