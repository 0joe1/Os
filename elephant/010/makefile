BUILD   := build
KERNEL  := kernel
DEVICE 	:= device
LIBK    := lib/kernel
LIBU    := lib/usr

#LDFALGS := -Ttext 0xc0001500 -e main
ASFLAGS = -f elf -w-orphan-label -g
CFLAGS  = -c -fno-builtin -m32 -fno-stack-protector -g

OBJS_BIN=mbr.bin loader.bin $(BUILD)/kernel.bin

#image: ${OBJS_BIN} 定义了一个名为image的目标，依赖于OBJS_BIN，
#表示要生成一个名为boot.img的磁盘映像文件，并将mbr.bin写入该磁盘映像中。
image: ${OBJS_BIN}
#创建一个大小为30MB的全0磁盘映像文件。
	dd if=/dev/zero of=boot.img bs=512 count=61440
#mbr.bin文件写入boot.img磁盘映像文件的第0个扇区。
	dd if=mbr.bin of=boot.img bs=512 count=1 conv=notrunc
#loader.bin文件写入boot.img磁盘映像文件的第2个扇区。
	dd if=loader.bin of=boot.img bs=512 count=3 seek=2 conv=notrunc
#kernel.bin文件写入boot.img磁盘映像文件的第9个扇区。
	dd if=$(BUILD)/kernel.bin of=boot.img bs=512 count=200 seek=9 conv=notrunc
#@-表示忽略删除过程中出现的错误。
	@-rm -rf *.bin	
  
OBJS_O = $(BUILD)/main.o $(BUILD)/init.o $(BUILD)/interrupt.o\
		 $(BUILD)/print.o $(BUILD)/kernel.o $(BUILD)/timer.o

build: ${OBJS_O}
	ld -m elf_i386 -T kernel.ld $(OBJS_O)  -o $(BUILD)/kernel.bin

%.bin:%.asm
	nasm -I include/ $^ -o $@
$(BUILD)/%.o: $(LIBK)/%.S
	nasm $(ASFLAGS)  $^ -o $@
$(BUILD)/%.o: $(KERNEL)/%.S
	nasm $(ASFLAGS)  $^ -o $@
$(BUILD)/%.o: $(KERNEL)/%.c
	gcc  $(CFLAGS) -I $(DEVICE)/ -I $(LIBK)/ -I $(KERNEL) $^ -o $@
$(BUILD)/%.o: $(DEVICE)/%.c
	gcc  $(CFLAGS) -I $(KERNEL)/ -I $(LIBK)/ -I $(KERNEL) $^ -o $@

#gdb_symbol:
#	objcopy --only-keep-debug $(BUILD)/kernel.bin $(BUILD)/kernel.sym

#定义了一个名为run的目标
run:
	make build
	make gdb_symbol
	make image
	/home/username/bochs2.6/bin/bochs -f bochsrc.disk


clean:
	rm -rf *.img  *.out *.lock *.bin *.o kernel/*.o lib/kernel/*.o
.PHONY:clean image build gdb_symbol
