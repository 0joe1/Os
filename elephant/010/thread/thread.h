#ifndef THREAD_H
#define THREAD_H
#include "list.h"
#include "stdint.h"

#define KMAGIC 0x54878290  //防止栈破坏信息

typedef void thread_func(void*);

enum task_status{
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_READY,
    TASK_DIED
};


struct intr_stack{
    uint_32 vec_no;
    uint_32 edi;
    uint_32 esi;
    uint_32 ebp;
    uint_32 esp_dump;
    uint_32 ebx;
    uint_32 edx;
    uint_32 ecx;
    uint_32 eax;
    uint_32 gs;
    uint_32 fs;
    uint_32 es;
    uint_32 ds;

    uint_32 err_code;
    void (*eip)(void);
    uint_32 cs;
    uint_32 eflags;
    uint_32 esp;
    uint_32 ss;
};

struct thread_stack {
    uint_32 ebp;
    uint_32 ebx;
    uint_32 edi;
    uint_32 esi;

    void (*eip) (thread_func* ,void* );
    void* unused_retaddr;
    thread_func* func;
    void* arg;
};

struct task_struct {
    uint_32* kstack_p;
    char* name[20];
    struct list_elm wait_tag;
    struct list_elm all_list_tag;
    uint_32 ticks;
    uint_32 elapsed_ticks;
    enum task_status status;
    uint_32 priority;
    uint_32 kmagic;
};

void init_thread(struct task_struct* pcb,char* name,uint_32 priority);
void thread_create(struct task_struct* pcb,thread_func* func,void* arg);
void thread_start(char* name,uint_32 priority,thread_func* func,void* arg);
void* running_thread(void);
void schedule(void);
void make_main_thread(void);
void thread_init(void);

#endif
