;主引导程序
;---------------------------------------------------------
SECTION MBR vstart=0x7c00
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov ss,ax
    mov sp,0x7c00


;---------------------------------------------------------
;INT 0x10 功能号:0x06 功能描述:上卷窗口
;---------------------------------------------------------
;input
;ah=功能号,al=上卷行数,bh=上卷行属性
;(cl,ch)= 窗口左上角(X,Y)
;(dl,dh)= 窗口右下角(X,Y)
;no return
    mov ax,0x600
    mov cx,0x00
    mov dx,0x184f
    mov bx,0x700

    int 0x10

;--------------获取光标位置-------------------------------
;input: 功能号3,bh=光标页号
;output:  dh=光标所在行号，dl=光标所在列号
    mov ah,3
    mov bh,0

    int 0x10

;--------------打印字符串---------------------------------
;功能号0x13,al=写字符方式,bh=页号,bl=字符属性
    mov ax,message
    mov bp,ax

    mov cx,5          ;cx为串长度
    mov ax,0x1301

    mov bx,0x2

    int 0x10

    jmp $

    message db "1 MBR"
    times (510-($-$$)) db 0
    db 0x55,0xaa
