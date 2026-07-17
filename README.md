# MIT 6.1810 Lab 1: Xv6 and Unix Utilities

该分支是我在MIT6.1810实验中lab1的实验代码.

---

## 实验概览

本实验的核心内容是实现基于xv6系统实现一系列的Unix程序,包括`sleep`、`sixfive`、`memdump`、`find`等。通过实现这些程序来熟悉xv6系统用户态编程以及`write`,`fork`,`exec`等系统调用接口的使用。

---
## 开发环境
- **主机系统**：Windows 11 + WSL2（Ubuntu24.04）
```bash
$ sudo apt-get update && sudo apt-get upgrade
$ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

## 快速开始
```bash
# 构建并运行 xv6
make qemu

# 运行官方测试
make grade

# 针对单个任务进行测试
make GRADEFLAGS=name grade
```
---

## 已实现的功能

### sleep(easy)
**涉及文件: `user/sleep.c`**

**使用方法:**
```bash
sleep <seconds>
```
- seconds:需要睡眠的时长，需要为正整数.

**功能:** 让当前进程**睡眠指定的时间**，单位为两次计时器中断的间隔时间,由xv6系统定义.

### sixfive(moderate)
**涉及文件: `user/sixfive.c`**

**使用方法:**
```bash
sixfive <filename1> <filename2> ...
```
- filename: 需要检查的文件名.

**功能:** 负责打印指定文件中所有值为**6或5的倍数**的**连续数字**.其中连续数字必须包含在**两个分隔符中间**,不可被打断,分隔符包括`" -\r\t\n./,"`,注意空格是分隔符,文件的开头和结尾也算一个分隔符.

例如`"12-3a8/2//29 33\n5"`中包含连续数字12,2,29,33,5

### memdump(easy)
**涉及文件: `user/memdump.c`**

**使用方法:**
```bash
find <directory> <string1> <string2> ... 
```
- directory: 需要查找的目录
- string: 需要查找的文件名

**功能:** 传入参数`char *fmt`和`char *data`,根据`fmt`的格式打印`data`的内容

fmt的格式包括:
- `i`: 打印32bit的十进制数
- `p`: 打印64bit的十六进制数
- `h`: 打印16bit的十进制数
- `c`: 打印8bit的字符
- `s`: 打印8字节的字符串
- `S`: 打印字符串，以`\0`结尾

### find(moderate)
**涉及文件: `user/find.c`**

**功能:** 负责在指定目录下查找所有与指定文件名相同的文件，并打印出它们的路径。

### find -exec 扩展(moderate)
**涉及文件: `user/find.c`**

**使用方法:**
```bash
find <directory> <string1> <string2> ... -exec <command> <arg1> <arg2> ...
```
- directory: 需要查找的目录
- string: 需要查找的文件名
- command: 需要执行的指令
- arg: 需要传入指令的参数

**功能:** 为`find`程序添加`-exec`选项,执行`-exec`后面的指令，同时`find`找到的路劲将不再打印出来，而是作为该指令的参数传入执行。

例如
```bash
find . hello -exec echo we find
```
假设find找到了`./hello`和`./dir/hello`,这两个将作为`echo`的参数，最终执行：
```bash
echo we find ./hello ./dir/hello
```

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)


## 测试结果
以下是`make grade`的测试结果
```bash
== Test sleep, no arguments ==
$ make qemu-gdb
sleep, no arguments: OK (3.3s)
== Test sleep, returns ==
$ make qemu-gdb
sleep, returns: OK (0.8s)
== Test sleep, makes syscall ==
$ make qemu-gdb
sleep, makes syscall: OK (1.1s)
== Test sixfive_test ==
$ make qemu-gdb
sixfive_test: OK (0.9s)
== Test sixfive_readme ==
$ make qemu-gdb
sixfive_readme: OK (1.3s)
== Test sixfive_all ==
$ make qemu-gdb
sixfive_all: OK (1.3s)
== Test memdump, examples ==
$ make qemu-gdb
memdump, examples: OK (0.6s)
== Test memdump, format ii, S, p ==
$ make qemu-gdb
memdump, format ii, S, p: OK (0.8s)
== Test find, in current directory ==
$ make qemu-gdb
find, in current directory: OK (1.2s)
== Test find, in sub-directory ==
$ make qemu-gdb
find, in sub-directory: OK (1.1s)
== Test find, recursive ==
$ make qemu-gdb
find, recursive: OK (1.1s)
== Test exec ==
$ make qemu-gdb
exec: OK (0.8s)
== Test exec, multiple args ==
$ make qemu-gdb
exec, multiple args: OK (0.7s)
== Test exec, recursive find ==
$ make qemu-gdb
exec, recursive find: OK (1.4s)
== Test time ==
time: OK
Score: 131/131
```