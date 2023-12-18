%include "boot.inc"
SECTION loader
        mov byte[gs:0x00],'L'
        mov byte[gs:0x01],0xc2

        mov byte[gs:0x02],'O'
        mov byte[gs:0x03],0xc2

        mov byte[gs:0x04],'A'
        mov byte[gs:0x05],0xc2

        mov byte[gs:0x06],'D'
        mov byte[gs:0x07],0xc2

        mov byte[gs:0x08],'E'
        mov byte[gs:0x09],0xc2

        mov byte[gs:0x0a],'R'
        mov byte[gs:0x0b],0xc2

        jmp $
