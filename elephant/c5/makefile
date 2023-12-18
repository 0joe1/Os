OBJS_BIN=mbr.bin loader.bin kernel.bin

LDFALGS := -Ttext 0xc0001500 -e main

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
	dd if=kernel.bin of=boot.img bs=512 count=200 seek=9 conv=notrunc
#@-表示忽略删除过程中出现的错误。
#@-rm -rf *.bin	

#表示将以.asm为扩展名的汇编源文件编译为以.bin为扩展名的二进制文件。
%.bin:%.asm
	nasm -I include/ $^ -o $@
%.o:%.c
	gcc -m32 -c $^ -o $@
%.bin:%.o
	ld -m elf_i386 -T kernel.ld $^ -o $@

#定义了一个名为run的目标，依赖于OBJS_BIN，表示要运行程序。
run: ${OBJS_BIN}
	make image
	/home/username/bochs2.6/bin/bochs -f bochsrc.disk


clean:
	rm -rf *.img  *.out *.lock *.bin *.o

.PHONY:clean image
