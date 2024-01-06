section .data
str_c_lib: db "c library says hello world!",0xa
str_c_lib_len equ $-str_c_lib

str_syscall: db "syscall says: hello world!",0xa
str_syscall_len equ $-str_syscall

section .text
global _start
_start:
;;;;;;;;;;;;;; 方式1：模拟C语言中系统调用库函数write ;;;;;;;;;;;;;;;;;
    push str_c_lib_len
    push str_c_lib
    push 1

    call simu_write
    add esp,12

;;;;;;;;;;;;;; 方式2：跨过库函数，直接进行系统调用 ;;;;;;;;;;;;;;;;;;;
    mov eax,4
    mov ebx,1
    mov ecx,str_syscall
    mov edx,str_syscall_len
    int 0x80
;;;;;;;;;;;;;; 退出程序 ;;;;;;;;;;;;;;;;;;;
    mov eax,1 ;第一号子功能是 exit
    int 0x80

;;;;;;;;; 下面用simu_write 模拟C库中系统调用函数write ;;;;;;;;;;;;;;;;
simu_write:
    push ebp
    mov ebp,esp
    mov eax,4
    mov ebx,[ebp+8]
    mov ecx,[ebp+12]
    mov edx,[ebp+16]
    int 0x80
    pop ebp
    ret
