#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H
#include "thread.h"

#define DEFALT_PRI 31

#define USR_STACK_VADDR (0xc0000000-1000)
#define USR_VADDR_START 0x8048000
#define DIV_ROUND_UP(divd,divs) ((divd+divs-1)/divs)

void start_process(void* arg);
void create_page_dir(struct task_struct* pcb);
void usr_vaddr_init(struct task_struct* pcb);
void process_execute(char* name,void* filename);
void page_dir_activate(struct task_struct*);
void process_activate(struct task_struct*);

#endif
