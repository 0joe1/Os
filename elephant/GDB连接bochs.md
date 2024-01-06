## GDB连接bochs

在bochsrc.disk中加入这么一行：

```
gdbstub:enabled=1,port=1234,text_base=0,data_base=0,bss_base=0
```



配置bochs的config文件，编译，安装

```
./configure --prefix=/home/username/bochs-debug --enable-gdb-stub --enable-iodebug --enable-x86-debugger --with-x --with-x11 LDFLAGS='-pthread'
```

ps: `--enable-gdb-stub` 与 `--enabledebugger` 和 `--enable-disasm`是相互冲突的。

```
make
make install
```



之后开启bochs会出现等待gdb连接的字样

再打开一个新的窗口，启动gdb输入命令：`target remote localhost:1234` 以连接bochs进行调试

输入`symbol-file kernel.sym` 以加载kernel.bin的符号表

这个kernel.sym由是从kernel.bin中提取的符号表

```
objcopy --only-keep-debug $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/kernel.sym
```



若连接成功的话，bochs会停在BIOS的第一条指令上(not the bootloader nor the kernel).