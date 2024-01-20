#include "thread.h"
#include "string.h"
#include "memory.h"

#define PAGESIZE 4096

void kernel_thread(thread_func* func,void* arg){
    func(arg);
}

void init_thread(struct task_struct* pcb,char* name,uint_32 priority)
{
    pcb->kstack_p = (void*)((uint_32)pcb + PAGESIZE);
    strcpy((char*)pcb->name,name);
    pcb->priority = priority;
    pcb->status   = TASK_RUNNING;
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
    asm volatile("movl %0,%%esp;pop %%ebp;pop %%ebx;pop %%edi; \
                 pop %%esi;ret"::"g"(pcb->kstack_p):"memory");
}
