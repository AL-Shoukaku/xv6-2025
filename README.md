# MIT 6.1810 Lab 2: Syscall

该分支是我在MIT6.1810实验中lab2的实验代码.

---

## 实验概览

本实验的核心内容是熟悉用 gdb 调试 xv6 内核的方法以及在 xv6 中实现系统调用的整个流程，最后用一个攻击 xv6 的任务来展现内核 bug 可能会带来的严重后果。

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

### 3.Sandbox with allowed pathnames(easy)

这一部分是让`sandbox`支持`path`参数

```bash
sandbox <mask> <path> <cmd> <arg1> <arg2> ...
```
- `path`:指定安全路径

如果系统调用为`open()`或`exec()`，且被掩码`mask`所禁止，同时该调用的路径与`path`相同，则应当允许该调用的执行。

### 4.Attack xv6 (moderate)

**涉及文件：`user/secret.c`,`user/attack.c`,`kernel/vm.c`,`kernel/kalloc.c`**

在`kernel/vm.c`和`kernel/kalloc.c`中有三处在 lab2 中编译时会被省略掉的代码，这回导致内核在释放和申请内存页面时不会对页面内容进行清空。

#### secret

```bash
secret <content>
```
- `content`:要进行保密的内容。

`secret`指令会申请 8 * 4096 bit的内存空间，并在最开始加入`"This may help."`，随后放入`content`的内容

#### attack

```bash
attack
```

`attack`命令会输出上一次`secret`命令中的`content`内容，**一般要使用两次才能生效**。


---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)


## 测试结果
以下是`make grade`的测试结果
```bash
== Test answers-syscall.txt ==
answers-syscall.txt: OK
== Test sandbox_mask ==
$ make qemu-gdb
sandbox_mask: OK (2.9s)
== Test sandbox_fork ==
$ make qemu-gdb
sandbox_fork: OK (0.3s)
== Test sandbox_path ==
$ make qemu-gdb
sandbox_path: OK (1.1s)
== Test sandbox_most ==
$ make qemu-gdb
sandbox_most: OK (0.6s)
== Test sandbox_minus ==
$ make qemu-gdb
sandbox_minus: OK (1.1s)
== Test attack ==
$ make qemu-gdb
attack: OK (1.0s)
== Test time ==
time: OK
Score: 45/45
```