#include "memory.h"
#include "string.h"
#include "print.h"
#include "debug.h"
#include "thread.h"
#include "sync.h"

#define PAGE_SIZE 4096
#define BTMP_START 0xc009a000

#define K_HEAP_START 0xc0100000

#define PDE_IDX(addr) ((addr & 0xffc00000)>>22)
#define PTE_IDX(addr) ((addr & 0x003ff000)>>12)
#define PG_P 1
#define PG_RW_R 0
#define PG_RW_RW 2
#define PG_US_S 0
#define PG_US_U 4

struct pool kernel_pool,user_pool;
struct virt_addr ker_vaddr;

void mem_pool_init(uint_32 total_mem)
{
    put_str("  mem pool init start\n");
    uint_32 used_mem = 0x100000 + PAGE_SIZE*256;
    uint_32 free_mem = total_mem - used_mem;

    uint_32 all_free_pages = free_mem / PAGE_SIZE;
    uint_32 ker_free_pages  = all_free_pages / 2;
    uint_32 usr_free_pages  = all_free_pages - ker_free_pages;

    uint_32 kbm_len = ker_free_pages / 8; //管理的区域小于实际区域
    kernel_pool.btmp.bits = (uint_8*)BTMP_START;
    kernel_pool.btmp.map_size = kbm_len;
    kernel_pool.paddr_start = used_mem;
    kernel_pool.pool_size = ker_free_pages * PAGE_SIZE;
    lock_init(&kernel_pool.lock);

    uint_32 ubm_len = usr_free_pages / 8;
    user_pool.btmp.bits = (uint_8*)(BTMP_START + kbm_len);
    user_pool.btmp.map_size = ubm_len;
    user_pool.paddr_start = used_mem + ker_free_pages*PAGE_SIZE;
    user_pool.pool_size = usr_free_pages * PAGE_SIZE;
    lock_init(&user_pool.lock);

    bit_init(&kernel_pool.btmp);
    bit_init(&user_pool.btmp);

    put_str("kernel bitmap start:");
    put_int((uint_32)kernel_pool.btmp.bits);
    put_char('\n');
    put_str("kernel physical addr start:");
    put_int(kernel_pool.paddr_start);
    put_char('\n');

    put_str("user bitmap start:");
    put_int((uint_32)user_pool.btmp.bits);
    put_char('\n');
    put_str("user physical addr start:");
    put_int(user_pool.paddr_start);
    put_char('\n');

    ker_vaddr.btmp.bits = (uint_8*)(BTMP_START + kbm_len + ubm_len);
    ker_vaddr.btmp.map_size = ker_free_pages / 8;
    ker_vaddr.vaddr_start = K_HEAP_START;
    bit_init(&ker_vaddr.btmp);
    put_str("kernel virtural memory bitmap start:");
    put_int((uint_32)ker_vaddr.btmp.bits);
    put_char('\n');
    put_str("kernel virtural memory addr start:");
    put_int(ker_vaddr.vaddr_start);
    put_char('\n');
    put_str("  mem pool init done\n");
}

void mem_init(void)
{
    put_str("memory init start\n");
    uint_32 tot_mem = *((uint_32*)0x800);
    mem_pool_init(tot_mem);
    put_str("memory init done\n");
}

static void* get_vaddr(enum pool_flag pf,uint_32 pcnt)
{
    uint_32 vaddr_get;
    if (pf == PF_KERNEL)
    {
        uint_32 bit_start = bit_scan(&ker_vaddr.btmp,pcnt);
        if (bit_start == -1){
            return NULL;
        }

        uint_32 cnt = 0;
        while (cnt < pcnt){
            bitmap_set(&ker_vaddr.btmp,bit_start+cnt++,1);
        }
        vaddr_get = ker_vaddr.vaddr_start + bit_start*PAGE_SIZE;
    }
    else
    {
        //用户内存池，以后再实现
    }
    return (void*)vaddr_get;
}

uint_32* pte_ptr(uint_32 vaddr)
{
    uint_32 v = (0xffc00000 + (PDE_IDX(vaddr)<<12) + (PTE_IDX(vaddr)<<2));
    return (uint_32*)v;
}

uint_32* pde_ptr(uint_32 vaddr)
{
    uint_32* v = (uint_32*)(0xfffff000 + (PDE_IDX(vaddr)<<2));
    return v;
}

static void* palloc(struct pool* m_pool)
{
    uint_32 bit_off = bit_scan(&m_pool->btmp,1);
    if (bit_off == -1){
        return NULL;
    }
    bitmap_set(&m_pool->btmp,bit_off,1);

    return (void*)(m_pool->paddr_start + bit_off*PAGE_SIZE);
}

static void page_table_add(void* _vaddr,void* _paddr)
{
    uint_32 vaddr=(uint_32)_vaddr,paddr=(uint_32)_paddr;
    uint_32* pde = pde_ptr(vaddr),*pte = pte_ptr(vaddr);

    if ((*pde & PG_P) == 1)
    {
        ASSERT((*pte & PG_P)==0);
        *pte = paddr | PG_P | PG_RW_RW | PG_US_U;
    }
    else
    {
        void* new_phy_page = palloc(&kernel_pool);
        *pde = (uint_32)new_phy_page | PG_P | PG_RW_RW | PG_US_U;
        memset((void*)((uint_32)pte & 0xfffff000),0,PAGE_SIZE);

        ASSERT((*pte & PG_P)==0);
        *pte = paddr | PG_P | PG_RW_RW | PG_US_U;
    }
}

void* malloc_page(enum pool_flag pf,uint_32 pcnt)
{
    ASSERT(pcnt>0 && pcnt<64000); //按照用户和内核各250MB来计算
    uint_32 cnt = pcnt;
    struct pool* m_pool = pf==PF_KERNEL? &kernel_pool : &user_pool;
    void* vaddr_start = get_vaddr(pf,pcnt);
    if (vaddr_start == NULL){
        return NULL;
    }

    void* vaddr = vaddr_start;
    while (cnt--)
    {
        void* paddr = palloc(m_pool);
        if (paddr == NULL) {
            return NULL;
        }
        page_table_add(vaddr,paddr);
        vaddr =(void*)((uint_32)vaddr + PAGE_SIZE);
    }

    return vaddr_start;
}

void* get_kernel_pages(uint_32 pcnt)
{
    void* vaddr = malloc_page(PF_KERNEL,pcnt);
    if (vaddr != NULL)
        memset(vaddr,0,PAGE_SIZE*pcnt);
    return vaddr;
}

void* get_a_page(enum pool_flag pf,uint_32 vaddr)
{
    struct pool* m_pool = pf==PF_KERNEL? &kernel_pool : &user_pool;
    lock_acquire(&m_pool->lock);

    struct task_struct* cur = running_thread();
    if (pf == PF_KERNEL && cur->pdir == NULL)
    {
        uint_32 bit_idx = (vaddr - ker_vaddr.vaddr_start)/PAGESIZE;
        bitmap_set(&ker_vaddr.btmp,bit_idx,1);
    }
    else if (pf == PF_USER && cur->pdir != NULL)
    {
        uint_32 bit_idx = (vaddr - cur->usrprog_vaddr.vaddr_start)/PAGESIZE;
        bitmap_set(&cur->usrprog_vaddr.btmp,bit_idx,1);
    }
    else {
        PANIC("get a page");
    }

    void* paddr = palloc(m_pool);
    if (paddr == NULL){
        return NULL;
    }
    page_table_add((void*)vaddr,paddr);
    lock_release(&m_pool->lock);
    return (void*)vaddr;
}


uint_32 v2p(void* vaddr)
{
    uint_32 vr = (uint_32)vaddr;
    uint_32* pte = pte_ptr(vr);
    return ((*pte&0xfffff000) | (vr&0xfff));
}
