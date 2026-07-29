# MIT 6.1810 Lab 2: Syscall

该分支是我在MIT6.1810实验中lab2的实验代码.

---

## 实验概览

本实验的核心内容是

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

### 1.Using gdb(easy)

**涉及文件：answers-syscall.txt**

这部分主要是学习使用 gdb 来调试 xv6 系统，并回答 6 个问题，问题与答案已经写在`answers-syscall.txt`中。

#### 使用 gdb 来调试 xv6

首先在一个窗口 A 里运行`make qemu-gdb`来以调试模式来启动 xv6，然后在**另一个**窗口 B 里运行`gdb-multiarch`来连接到 xv6 的调试端口即可开始调试。

### 2.Sandbox a command(moderate)

**涉及文件：`user/sandbox.c`，`user/user.h`，`user/usys.pl`，`kernel/syscall.h`，`kernel/sysproc.c`，`kernel/proc.h`，`kernel/proc.c`，`kernel/syscall.c`,`Makefile`**


#### sandbox指令

**使用方法：**

```bash
sandbox <mask> <path> <cmd> <arg1> <arg2> ...
```

- `mask`:掩码，用于指定被禁用的系统调用。
- `path`:在本处恒为`-`
- `cmd`:执行的命令名字
- `arg`:命令`cmd`的参数

**具体功能：**

执行指令`cmd`，并在执行过程中禁用掩码`mask`所指定的系统调用，其中`mask`的第 i 位为 1 则表示系统调用号为 i 的系统调用被禁用

如果使用了被禁止的系统调用，则会产生错误并退出。


#### interpose()系统调用

定义于`user/user.h`中，形式如下：

```C
int interpose(int mask,const char *path);
```

`mask`为掩码，`path`在此处恒定为`-`，用于指定该进程禁用的系统调用，成功时返回 0 .

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)


## 测试结果
以下是`make grade`的测试结果
```bash


```