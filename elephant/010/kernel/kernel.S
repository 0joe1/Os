    [bits 32]
    %define ERR_CODE nop
    %define NONE push 0

    section .data
    int_msg db 'interupt occur',0x0a,0

    extern put_str  ;利用extern引入外部符号
    extern idt_table 

    %macro VECTOR 2

    section .text
    int%1entry:
        %2
        push ds
        push es
        push fs
        push gs
        pushad

        push %1
        call [idt_table+%1*4]
        add esp,4

        mov al,0x20
        out 0x20,al
        out 0xa0,al

        jmp int_exit

    section .data
        dd int%1entry

    %endmacro

section .text
global int_exit
int_exit:
    popad
    pop gs
    pop fs
    pop es
    pop ds

    add esp,4
    iret

section .data         ;idt_desc_addr需要是.data才能与后面的data合并到一个segment
global idt_desc_addr
idt_desc_addr:
    VECTOR 0x00,NONE
    VECTOR 0x01,NONE
    VECTOR 0x02,NONE
    VECTOR 0x03,NONE
    VECTOR 0x04,NONE
    VECTOR 0x05,NONE
    VECTOR 0x06,NONE
    VECTOR 0x07,NONE
    VECTOR 0x08,ERR_CODE
    VECTOR 0x09,NONE
    VECTOR 0x0a,NONE
    VECTOR 0x0b,NONE
    VECTOR 0x0c,NONE
    VECTOR 0x0d,NONE
    VECTOR 0x0e,NONE
    VECTOR 0x0f,NONE
    VECTOR 0x10,NONE
    VECTOR 0x11,ERR_CODE
    VECTOR 0x12,NONE
    VECTOR 0x13,NONE
    VECTOR 0x14,NONE
    VECTOR 0x15,NONE
    VECTOR 0x16,NONE
    VECTOR 0x17,NONE
    VECTOR 0x18,NONE
    VECTOR 0x19,NONE
    VECTOR 0x1a,NONE
    VECTOR 0x1b,NONE
    VECTOR 0x1c,NONE
    VECTOR 0x1d,NONE
    VECTOR 0x1e,NONE
    VECTOR 0x1f,NONE
    VECTOR 0x20,NONE
