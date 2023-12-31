;----------------------loader start-----------------------
LOADER_BASE_ADDR equ 0x600
LOADER_START_SECTION equ 0x02

;-------------------------gdt-----------------------------
DESC_G_4k equ 1_00000000000000000000000b
DESC_D    equ  1_0000000000000000000000b
DESC_L    equ    000000000000000000000b  ;32位
DESC_P    equ      1_000000000000000b

DESC_LIMIT_CODE    equ  1111_0000000000000000b
DESC_LIMIT_DATA    equ  DESC_LIMIT_CODE
DESC_LIMIT_VIDEO   equ  0000_0000000000000000b

DESC_DPL0   equ    000000000000000b
DESC_DPL1   equ    1_0000000000000b
DESC_DPL2   equ    11_0000000000000b

DESC_S_SYS  equ    0_000000000000b
DESC_S_NMR  equ    1_000000000000b ;系统段为0,数据段为1

TYPE_X    equ  1000_00000000b
TYPE_W    equ  0010_00000000b

DESC_CODE_HIGH4  equ DESC_G_4k + DESC_D + DESC_L + DESC_LIMIT_CODE + \
                     DESC_P + DESC_DPL0 + DESC_S_NMR + TYPE_X

DESC_DATA_HIGH4  equ DESC_G_4k + DESC_D + DESC_L + DESC_LIMIT_DATA + \
                     DESC_P + DESC_DPL0 + DESC_S_NMR + TYPE_W

DESC_VIDEO_HIGH4 equ DESC_G_4k + DESC_D + DESC_L + DESC_LIMIT_VIDEO + \
                     DESC_P + DESC_DPL0 + DESC_S_NMR + TYPE_W + 0x0b

;--------------------DPL and Ti-------------------------
DPL_0     equ  00b
DPL_1     equ  01b
DPL_2     equ  10b
TI_GDT    equ  000b
TI_LDT    equ  100b


