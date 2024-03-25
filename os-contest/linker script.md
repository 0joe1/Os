# linker script

### 一些基本概念

每个段都有名字和大小。

 *loadable* ，input section content 被 load 到 object file 的相应 output section

*allocatable* ，预留着但没有信息（有些需要全清为零）

A section 非上面两者包含debug信息。

`objdump -h` 查看sections



Every loadable or allocatable output section has two addresses，分别是VMA(virtual memory address)和LMA(load memory address) 。这俩在大多数情况下都是相同的。

一个相同的情况：一个data section被加载到ROM，程序启动时被copy到RAM中。这时加载到ROM的地址是LMA，copy到RAM的地址是VMA。



每个 object file 都有 符号表，defined 函数 和 global 和static变量有defined symbol；若undefined有undefined symbol。

`objdump -t` or `nm` 可以查看这些符号。



### 示例

keyword ‘SECTIONS’ 后跟一系列 symbol assignments 和 output section descriptions ，它们被大括号所包围。

给一个 output section descriptions 的例子：

左边是 output section ，右边是 input section

```
SECTIONS
{
  . = 0x10000;
  .text : { *(.text) }
  . = 0x8000000;
  .data : { *(.data) }
  .bss : { *(.bss) }
}
```

`*`是通配符，` { *(.text) }`表示input file 中所有的 `.text` section。

当前 output section 的 address = location counter。为了对齐，location counter 在加上当前 output sections 大小的基础上还要多加一点。两个 output sections 之间会有gap。





###  Assigning Values to Symbols

你可以在 linker script 里为 symbol 赋值，这会符号表中定义一个全局符号。

在写表达式赋值语句时，可以把它们作为单独的部分,也可以作为 ’*SECTIONS*’ 命令中的一个语句,或者作为 ’*SECTIONS*’ 命令中输出段描述的一个部分。

符号的有效作用区域由表达式所在的段决定。

```
floating_point = 0;
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
    }
  _bdata = (. + 3) & ~ 3;
  .data : { *(.data) }
}
```

特殊符号名称 `.` 表示位置计数器，只能在 *SECTIONS* 命令中使用。比如说这里 `_etext` 的值等于紧随 `.text` 最后一个input section 的地址。





#### PROVIDE

只有在程序内没定义该符号才定义。

```
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
      PROVIDE(etext = .);
    }
}
```

比如这个，若程序中有 `_etext` ，会给出多重定义错误；若程序中有`etext`，会采用程序中的定义。





### KEEP

保持 section 中的symbol，即使它们没有被定义。

某个 section 引用其他 section中的符号，这样那个被引用的 section 就被引用了（好像是废话）。那么，我们显然需要保证原初是有一个 root 一定是存在的，它来引用其他的 section ，那些被引用的 section 又引用下一级 sections，这样最终形成一个树。所以 KEEP 常常用在依赖树的根。