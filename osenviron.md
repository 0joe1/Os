# 比赛环境配置
## 拉取docker镜像
>docker pull docker.educg.net/cg/os-contest:2024p4
## 启动docker
>docker run --privileged -it --rm -v /lib/modules:/lib/modules  --name os-contest -p 12306:22 os-contest  /bin/bash

base 镜像底层直接用 Host 的 kernel，自己只需要提供 roofs。docker 凭借 namespace实现容器资源的隔离，而module不包含在此，因此使用 kernel 的module。

Docker 有一种存储机制，叫做 Data Volume 。本质上就是把Docker Host 文件系统中的目录或文件 mount 到容器的文件系统中去。

这里的`-v  <host path>:<container path>` 采用 bind mount 方式将host中文件mount到容器，与 Linux mount 的行为一致。



## 启动qemu

* 生成2kfs.img\
在启动qemu之前要先运行脚本create_qemu_img.sh生成2kfs.img文件。\
原create_qemu_img.sh中内容：
```sh
if [ -f ./2kfs.img ] ; then
   echo "2kfs.img exist! removed"
   rm -f ./2kfs.img
fi

qemu-img create -f qcow2 2kfs.img 2G

if [ $? -ne 0 ] ; then
  echo "create image failed"
  exit -1
fi

sudo qemu-nbd -c /dev/nbd0 ./2kfs.img

if [ $? -ne 0 ] ; then
  echo "connect image to nbd device failed!"
  echo "please install nbd kernel module first!"
  echo "   modprobe nbd maxparts=12"
  echo "if /dev/nbd0 is already taken, change all nbd0 in this script to another one such as nbd1"
  exit -2
fi

sudo echo -e 'n\n\n\n\n\n\nw\nq\n'| sudo fdisk /dev/nbd0

if [ $? -ne 0 ] ; then
  echo "disk partition failed"
  exit -3
fi

sudo mkfs.ext4 /dev/nbd0p1

if [ $? -ne 0 ] ; then
  echo "mkfs.ext4 failed"
  exit -4
fi

sudo mount /dev/nbd0p1 /mnt

if [ $? -ne 0 ] ; then
  echo "mount /dev/nbd0p1 failed"
  exit -5
fi

sudo bash -c "lzcat /tmp/qemu/2k1000/rootfs-la.cpio.lzma | cpio -idmv -D /mnt &> ./cpio.log"

if [ $? -ne 0 ] ; then
  echo "unpack rootfs failed"
  exit -6
fi

sudo mkdir /mnt/boot 

sudo cp /tmp/qemu/2k1000/uImage /mnt/boot/

sudo umount /mnt

sudo qemu-nbd -d /dev/nbd0

echo "done"

```
在执行这个脚本之前先`modprobe nbd maxparts=12` 加载nbd模块

modprobe - program to add and remove modules from the Linux Kernel
--max_part limit 指定块设备允许的最大分区数量，某些版本的 Linux 不加 max_part 参数会导致没有设备节点 /dev/nbd0p{1,2,3,4…} 等。

但是，等等。。。还不能运行该脚本，先往下看



## nbd 模块

**网络块设备**（Network Block Device, NBD）是一种[设备节点](https://zh.wikipedia.org/w/index.php?title=设备节点&action=edit&redlink=1)，其内容由远程计算机提供。网络块设备通常用于访问非物理安装于本地计算机上，而在远程的存储设备。例如，本地计算机可访问连接于另一台计算机上的[硬盘](https://zh.wikipedia.org/wiki/硬盘)。NBD提供的是一个块设备（*块设备*是指与系统间用块的方式移动数据的设备）。客户端可以把这个块设备格式化成各种类型的分区，更便于用户的使用。



**qemu-nbd** [*OPTION*]… *filename*

使用 NBD 协议导出 QEMU 磁盘映像。

-c, --connect=DEV
将文件名连接到 NBD 设备 DEV（仅限 Linux）。



照上面所述，/dev 下应该有nbd0了，可是看了下并没有

```
rmmod nbd 
sudo mount -t devtmpfs none /dev 
sudo modprobe nbd maxparts=12
```

devtmpfs 允许在启动后添加或删除devices。我们可以在remount /dev 到 devtmpfs。

> [devtmpfs](https://www.zhihu.com/search?q=devtmpfs&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"answer"%2C"sourceId"%3A2286559645}) 在 Linux 核心 启动早期 创造tmpfs，建立一个初步的 /dev，令一般启动程序不用等待 udev。
>
> 在设备驱动注册之前，每一个设备都会在/tmpfs 中有一个节点。roots被 kernel 挂载后，tmpfs被挂载至 /dev
>
>  **udev** 是Linux kernel的设备管理器，主要管理`/dev`目录底下的[设备节点](https://zh.wikipedia.org/w/index.php?title=设备节点&action=edit&redlink=1)。动态提供了在系统中实际存在的设备节点。



### 用 loop 设备代替 nbd0(未尝试)

因为缺少nbd模块并不能通过create_qemu_img.sh直接生成2kfs.img。\
可以使用loop设备代替nbd0
查看可用的loop设备

>sudo losetup -f 

如果没有loop设备，可以使用创建一个
>sudo mknod -m 660 /dev/loop0 b 7 0

如果loop设备已经被使用
>sudo losetup -d /dev/loop0 

关联loop设备与镜像文件
>sudo losetup /dev/loop0 ./2kfs.img

填写分区表
>sudo echo -e 'n\n\n\n\n\n\nw\nq\n'| sudo fdisk /dev/nbd0

格式化文件系统
>sudo mkfs.ext4 -q /dev/loop0

将磁盘挂载到mnt目录
>sudo mount -o loop ./2kfs.image /mnt

将文件系统解压到mnt目录中
>sudo bash -c "lzcat /tmp/qemu/2k1000/rootfs-la.cpio.lzma | cpio -idmv -D /mnt &> ./cpio.log"

创建/boot目录并将uImage拷贝到目录中
> sudo cp /tmp/qemu/2k1000/uImage /mnt/boot/

* sdcard.img\
sdcard.img 文件为比赛给定的测试文件，需要下载到本地后再传入docker镜像中\
下载地址：https://github.com/oscomp/testsuits-for-oskernel/blob/pre-2024/sdcard-loongarch.img.gz\
下载好后可以通过docker cp指令将内容传输到docker中

准备好后可通过如下指令启动qemu
>qemu-system-loongarch64 -M ls2k -serial stdio -serial vc -drive if=pflash,file=/tmp/qemu/2k1000/u-boot-with-spl.bin -m 1024 -device usb-kbd,bus=usb-bus.0 -device usb-tablet,bus=usb-bus.0     -device usb-storage,drive=udisk -drive if=none,id=udisk,file=/tmp/disk -net nic -net user,net=10.0.2.0/24,tftp=/srv/tftp -vnc :0 -D /tmp/qemu.log     -s -hda /tmp/qemu/2k1000/2kfs.img -drive file=sdcard.img,if=none,format=raw,id=x0 \
启动时按c可进入uboot命令行
## 安装交叉编译工具链
下载地址：https://gitlab.educg.net/wangmingjian/os-contest-2024-image/-/blob/master/toolchain-loongarch64-linux-gnu-gcc8-host-x86_64-2022-07-18.tar.xz\
下载后解压文件并将解压好后的文件夹拷贝到/opt目录下
添加环境变量
移动文件夹之后要
打开zsh配置文件

>code ~/.zshrc

打开后在文件尾部添加
```sh
export PATH=$PATH:/opt/toolchain-loongarch64-linux-gnu-gcc8-host-x86_64-2022-07-18/bin
```
## qemu的网络配置（tftp已经试用过，启动不了）
启动docker后
在docker中安装

```
sudo apt-get install bridge-utils        # 虚拟网桥工具
sudo apt-get install uml-utilities       # UML（User-mode linux）工具
```





主要参考资料：

[网络块设备 wiki](https://zh.wikipedia.org/wiki/%E7%BD%91%E7%BB%9C%E5%9D%97%E8%AE%BE%E5%A4%87)

[linux qemu-nbd介绍](https://blog.csdn.net/hbuxiaofei/article/details/106732500)

[Loading Kernel Modules in a Docker Container](https://www.baeldung.com/linux/docker-container-kernel-modules)

每天五分钟玩转docker容器技术

[Modprobe doesn't create nbd device](https://serverfault.com/questions/787568/modprobe-doesnt-create-nbd-device)

[udev wiki](https://zh.wikipedia.org/wiki/Udev)

[driver-core: devtmpfs - driver core maintained /dev tmpfs](https://lwn.net/Articles/330985/)