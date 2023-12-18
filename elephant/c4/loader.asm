%include "boot.inc"
;----------------------------------------------------------
SECTION loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR
jmp  loader_start
        GDT_NULL dd 0x00
                 dd 0x00

        CODE_SEG  dd  0x0000ffff
                  dd  DESC_CODE_HIGH4

        DATA_SEG  dd  0x0000ffff
                  dd  DESC_DATA_HIGH4

        VIDIO_SEG dd  0x80000007
                  dd  DESC_VIDEO_HIGH4

        GDT_SIZE  equ  $-GDT_NULL
        GDT_LIMIT equ  GDT_SIZE-1

        times 60  dq  0          ;预留空位

        SELECTOR_CODE  equ (0x01<<3) + TI_GDT + DPL_0
        SELECTOR_DATA  equ (0x02<<3) + TI_GDT + DPL_0
        SELECTOR_VIDEO equ (0x03<<3) + TI_GDT + DPL_0

        gdt_ptr   dw  GDT_LIMIT
                  dd  GDT_NULL

        loader_msg  db  'loader in real'

;INT 0x10
;功能描述:打印字符串
;------------------------------------------------------------
;输入:
;AH 子功能号=13H
;BH = 页码
;BL = 属性(若 AL=00H 或 01H)
;CX=字符串长度
;(DH、 DL)=坐标(行、 列)
;ES:BP=字符串地址
;AL=显示输出方式
; 0—字符串中只含显示字符,其显示属性在 BL 中
;显示后,光标位置不变
; 1—字符串中只含显示字符,其显示属性在 BL 中
;显示后,光标位置改变
; 2—字符串中含显示字符和显示属性。显示后,光标位置不变
; 3—字符串中含显示字符和显示属性。显示后,光标位置改变
;无返回值

    loader_start:
        mov ax,cs
        mov es,ax
        mov ax,0x1301
        mov bx,0x00a4 ;绿底红字带闪烁
        mov bp,loader_msg
        mov dx,0x0200
        mov cx,14

        int 0x10

;------------------------------------------------------------
        ;A20
        in al,0x92
        or al,0000_0010b
        out 0x92,al

        ;加载gdtr寄存器
        lgdt [cs:gdt_ptr]

        ;进入保护模式
        mov eax,cr0
        or eax,1
        mov cr0,eax

        ;清空流水线并更新段描述符缓冲寄存器
        jmp SELECTOR_CODE:protect_mode
;------------------------------------------------------------
    [bits 32]
    protect_mode:
        mov ax,SELECTOR_DATA
        mov ds,ax
        mov ss,ax
        mov esp,LOADER_STACK_TOP
        mov ax,SELECTOR_VIDEO
        mov es,ax
        mov byte[es:160],'P'

        jmp $
