#include "thread.h"
#include "string.h"
#include "memory.h"
#include "list.h"
#include "debug.h"
#include "interrupt.h"
#include "switch.h"
#include "print.h"
#include "process.h"
#include "sync.h"
#include "fork.h"

#define MAIN_THREAD_PRIO 31

extern void init(void);

struct list thread_ready_list;
struct list all_thread_list;

struct lock pid_lock;
pid_t sys_pid;

struct task_struct* idle_pcb;

static void asign_pid(struct task_struct* pcb)
{
    lock_acquire(&pid_lock);
    sys_pid++;
    pcb->pid = sys_pid;
    lock_release(&pid_lock);
}
void fork_pid(struct task_struct* pcb){
    asign_pid(pcb);
}

void kernel_thread(thread_func* func,void* arg){
    intr_enable();   //没有iret，EFLAG未恢复，需手动置位IF
    func(arg);
}

void* running_thread()
{
    uint_32 pcb_addr;
    asm volatile("movl %%esp,%0":"=g"(pcb_addr));
    return (void*)((pcb_addr-1) & 0xfffff000);
}
void init_thread(struct task_struct* pcb,char* name,uint_32 priority)
{
    pcb->kstack_p = (void*)((uint_32)pcb + PAGESIZE);
    strcpy((char*)pcb->name,name);
    asign_pid(pcb);
    pcb->ppid = -1;
    pcb->priority = priority;
    pcb->ticks    = priority;
    pcb->elapsed_ticks = 0;
    pcb->status   = TASK_READY;

    for (uint_32 fd = 0 ; fd < MAX_OPEN_FILES_PROC ; fd++) {
        if (fd < 3) pcb->fd_table[fd]=fd;
        else pcb->fd_table[fd] = -1;   //不能是0，0是标准输入
    }
    pcb->cwd_inode_nr = 0;
    pcb->kmagic = KMAGIC;
}

void thread_create(struct task_struct* pcb,thread_func* func,void* arg)
{
    pcb->kstack_p = (uint_32*)((uint_32)pcb->kstack_p - sizeof(struct intr_stack));
    pcb->kstack_p = (uint_32*)((uint_32)pcb->kstack_p - sizeof(struct thread_stack));

    struct thread_stack* thread = (struct thread_stack*)pcb->kstack_p;
    thread->eip  = kernel_thread;
    thread->func = func;
    thread->arg  = arg;
    thread->esi = thread->edi = thread->ebp = thread->ebx = 0;
}

struct task_struct* thread_start(char* name,uint_32 priority,thread_func* func,void* arg)
{
    void* pcb_addr = get_kernel_pages(1);
    struct task_struct* pcb = (struct task_struct*)pcb_addr;
    init_thread(pcb,name,priority);
    thread_create(pcb,func,arg);

    list_append(&all_thread_list,&pcb->all_list_tag);
    list_append(&thread_ready_list,&pcb->wait_tag);
    return pcb_addr;
}

void schedule(void)
{
    ASSERT(get_intr_status() == INTR_OFF)
    struct task_struct* cur = running_thread();
    ASSERT(elem_find(&all_thread_list,&cur->all_list_tag));
    if (cur->ticks == 0)
    {
        ASSERT(!elem_find(&thread_ready_list,&cur->wait_tag));
        cur->ticks  = cur->priority;
        cur->status = TASK_READY;
        list_append(&thread_ready_list,&cur->wait_tag);
    }

    if (list_empty(&thread_ready_list)) thread_unblock(idle_pcb);
    struct list_elm* next_ready_tag = list_pop(&thread_ready_list);
    struct task_struct* next = mem2entry(struct task_struct,next_ready_tag,wait_tag);
    process_activate(next);
    next->status = TASK_RUNNING;
    switch_to(cur,next);
}

void make_main_thread()
{
    struct task_struct* main_pcb = running_thread();
    init_thread(main_pcb,"main_thread",21);
    main_pcb->status = TASK_RUNNING;
    ASSERT(!elem_find(&all_thread_list,&main_pcb->all_list_tag));
    list_append(&all_thread_list,&main_pcb->all_list_tag);
}

void thread_init(void)
{
    put_str("thread init start\n");
    lock_init(&pid_lock);
    list_init(&all_thread_list);
    list_init(&thread_ready_list);
    process_execute("init",init);
    make_main_thread();
    idle_pcb = thread_start("idle",10,idle,NULL);
    put_str("thread init done\n");
}

void thread_block(enum task_status stat)
{
    enum intr_status old_status = intr_disable();
    ASSERT(stat==TASK_BLOCKED || \
           stat==TASK_WAITING || \
           stat==TASK_HANGING );

    struct task_struct* cur = (struct task_struct*)running_thread();
    ASSERT(cur->status==TASK_RUNNING);
    cur->status = stat;
    schedule();
    intr_set_status(old_status);
}

void thread_unblock(struct task_struct* thread)
{
    enum intr_status old_status = intr_disable();
    ASSERT(thread->status==TASK_BLOCKED || \
           thread->status==TASK_WAITING || \
           thread->status==TASK_HANGING );

    thread->status = TASK_READY;
    if (elem_find(&thread_ready_list,&thread->wait_tag)) {
        PANIC("thread unblock: already in thread ready list");
    }
    list_push(&thread_ready_list,&thread->wait_tag);
    intr_set_status(old_status);
}

void idle(void* arg)
{
    while (1) {
        thread_block(TASK_BLOCKED);
        asm volatile("sti;hlt;":::"cc");
    }
}


void thread_yield(void)
{
    struct task_struct* cur = running_thread();
    enum intr_status old_status = intr_disable();
    cur->status = TASK_READY;
    list_append(&thread_ready_list,&cur->wait_tag);
    schedule();
    intr_set_status(old_status);
}


int_32 fdlocal2gloabl(int_32 local_fd)
{
    struct task_struct* cur = running_thread();
    int_32 _fd = cur->fd_table[local_fd];
    return _fd;
}

