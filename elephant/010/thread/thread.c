#include "thread.h"
#include "string.h"
#include "memory.h"
#include "list.h"
#include "debug.h"
#include "interrupt.h"
#include "switch.h"
#include "print.h"

#define PAGESIZE 4096
#define MAIN_THREAD_PRIO 31


struct list thread_ready_list;
struct list all_thread_list;

void kernel_thread(thread_func* func,void* arg){
    intr_enable();
    func(arg);
}

void* running_thread()
{
    uint_32 pcb_addr;
    asm volatile("movl %%esp,%0":"=g"(pcb_addr));
    return (void*)(pcb_addr & 0xfffff000);
}
void init_thread(struct task_struct* pcb,char* name,uint_32 priority)
{
    pcb->kstack_p = (void*)((uint_32)pcb + PAGESIZE);
    strcpy((char*)pcb->name,name);
    pcb->priority = priority;
    pcb->ticks    = priority;
    pcb->elapsed_ticks = 0;
    pcb->status   = TASK_READY;
    pcb->kmagic   = KMAGIC;
}

void thread_create(struct task_struct* pcb,thread_func* func,void* arg)
{
    pcb->kstack_p -= sizeof(struct intr_stack);
    pcb->kstack_p -= sizeof(struct thread_stack);

    struct thread_stack* thread = (struct thread_stack*)pcb->kstack_p;
    thread->eip  = kernel_thread;
    thread->func = func;
    thread->arg  = arg;
    thread->esi = thread->edi = thread->ebp = thread->ebx = 0;
}

void thread_start(char* name,uint_32 priority,thread_func* func,void* arg)
{
    void* pcb_addr = get_kernel_pages(1);
    struct task_struct* pcb = (struct task_struct*)pcb_addr;
    init_thread(pcb,name,priority);
    thread_create(pcb,func,arg);

    list_append(&all_thread_list,&pcb->all_list_tag);
    list_append(&thread_ready_list,&pcb->wait_tag);
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
    else
    {
        //阻塞，以后再实现
    }

    struct list_elm* next_ready_tag = list_pop(&thread_ready_list);
    struct task_struct* next = mem2entry(struct task_struct,next_ready_tag,wait_tag);
    next->status = TASK_RUNNING;
    switch_to(cur,next);
}

void make_main_thread()
{
    struct task_struct* main_pcb = running_thread();
    strcpy((char*)main_pcb->name,"main thread");
    main_pcb->kstack_p = (void*)((uint_32)main_pcb + PAGESIZE);
    main_pcb->status = TASK_RUNNING;
    main_pcb->priority = MAIN_THREAD_PRIO;
    main_pcb->ticks  = MAIN_THREAD_PRIO;
    main_pcb->elapsed_ticks = 0;
    list_append(&all_thread_list,&main_pcb->all_list_tag);
    main_pcb->kmagic = KMAGIC;
}

void thread_init(void)
{
    put_str("thread init start\n");
    list_init(&all_thread_list);
    list_init(&thread_ready_list);
    make_main_thread();
    put_str("thread init done\n");
}

