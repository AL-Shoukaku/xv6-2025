# MIT 6.1810 Lab 3: page tables

该分支是我在MIT6.1810实验中lab3的实验代码.

---

## 实验概览

本次实验的主题是 xv6 系统中的页表机制，主要包括**页表的打印与内容解析**，通过**共享页面**来加速系统调用，以及在在原有基础上实现**超级页**。

可以在`task.md`中查看整个实验内容。

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

### 1. Inspect a user-process page table (easy)

**涉及文件:`user/pgtbltest.c`,`answers-pgtbl.txt`**

这一部分要求在 xv6 中运行`pgtbltest`程序，它会打印出这个进程的前10页和最后 10 页的信息，我们需要根据此回答以下问题：

> For every page table entry in the print_pgtbl output, explain what it logically contains and what its permission bits are. Figure 3.4 in the xv6 book might be helpful, although note that the figure might have a slightly different set of pages than process that's being inspected here. Note that xv6 doesn't place the virtual pages consecutively in physical memory.

问题的解答在`answers-pgtbl.txt`中。

`pgtbltest`程序的输出如下：

```bash
va 0x0 pte 0x21FC885B pa 0x87F22000 perm 0x5B
va 0x1000 pte 0x21FC7C5B pa 0x87F1F000 perm 0x5B
va 0x2000 pte 0x21FC7817 pa 0x87F1E000 perm 0x17
va 0x3000 pte 0x21FC7407 pa 0x87F1D000 perm 0x7
va 0x4000 pte 0x21FC70D7 pa 0x87F1C000 perm 0xD7
va 0x5000 pte 0x0 pa 0x0 perm 0x0
va 0x6000 pte 0x0 pa 0x0 perm 0x0
va 0x7000 pte 0x0 pa 0x0 perm 0x0
va 0x8000 pte 0x0 pa 0x0 perm 0x0
va 0x9000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF6000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF7000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF8000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF9000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFA000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFB000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFC000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFD000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFE000 pte 0x21FD08C7 pa 0x87F42000 perm 0xC7
va 0x3FFFFFF000 pte 0x2000184B pa 0x80006000 perm 0x4B
```

### 2. Speed up system calls (easy)

**涉及文件：`user/ulib.c`,`kernel/proc.c`,`kernel/exec.c`**

这一部分我们需要优化 xv6 中`getpid()`系统调用的性能。原本`getpid()`系统调用需要进行两次用户态和内核态的切换，而在本体中我们需要对每一个进程都在地址`USYSCALL`(定义于`kernel/memlayout.h`)处映射一页代码，并在该页的起始处存储一个`struct usyscall`结构体，里面包含了进程的`pid`.

这样就可以使用在用户态下的`ugetpid()`函数直接读取`USYSCALL`地址处的`pid`，从而避免了陷入内核的开销。

在运行`pgtbltest`时，我们能看到以下输出：

```bash
print_kpgtbl starting
print_kpgtbl: OK
```

### 3. Print a page table (easy)

**涉及文件：`kernel/vm.c`**

实现`vmprint()`函数，打印当前进程的页表信息，输出如下：

```bash
print_kpgtbl starting
page table 0x0000000083f22000
 ..0x0000000000000000: pte 0x0000000020fc7801 pa 0x0000000083f1e000
 .. ..0x0000000000000000: pte 0x0000000020fc7401 pa 0x0000000083f1d000
 .. .. ..0x0000000000000000: pte 0x0000000020fc7c5b pa 0x0000000083f1f000
 .. .. ..0x0000000000001000: pte 0x0000000020fc705b pa 0x0000000083f1c000
 .. .. ..0x0000000000002000: pte 0x0000000020fc6cd7 pa 0x0000000083f1b000
 .. .. ..0x0000000000003000: pte 0x0000000020fc6807 pa 0x0000000083f1a000
 .. .. ..0x0000000000004000: pte 0x0000000020fc64d7 pa 0x0000000083f19000
 ..0x0000003fc0000000: pte 0x0000000020fc8401 pa 0x0000000083f21000
 .. ..0x0000003fffe00000: pte 0x0000000020fc8001 pa 0x0000000083f20000
 .. .. ..0x0000003fffffd000: pte 0x0000000020fd4853 pa 0x0000000083f52000
 .. .. ..0x0000003fffffe000: pte 0x0000000020fd00c7 pa 0x0000000083f40000
 .. .. ..0x0000003ffffff000: pte 0x000000002000184b pa 0x0000000080006000
print_kpgtbl: OK
```

第一行是页表**基地址**，剩下的每一行是一个**有效的**页表项，输出对应的**虚拟地址、页表项、物理地址**。

通过`".."`的数量来体现该页表项所处的层级。

此外还在`answers-pgblt.txt`中回答了如下问题：

> For every leaf page in the vmprint output, explain what it logically contains and what its permission bits are, and how it relates to the output of the earlier print_pgtbl() exercise above. Figure 3.4 in the xv6 book might be helpful, although note that the figure might have a slightly different set of pages than the process that's being inspected here.

### 4. Use superpages (moderate)/(hard)

**涉及文件：`kernel/defs.h`,`kernel/vm.c`,`kernel/kalloc.c`**

这一部分在 xv6 系统中实现了**超级页(superpage)**功能。

在`kernel/kalloc.c`中修改了内存的初始化逻辑，将128MB中的前64MB作为普通页的内存池，后64MB作为超级页的内存池。

在`kernel/kalloc.c`中实现了`superalloc()`,`superfree()`等用于**分配和释放**超级页的函数。

在`kernel/vm.c`中实现`supermappages()`,用于映射超级页

修改`kernel/vm.c`中`uvmalloc()`的逻辑，当内存需求大于 2MB 时，会使用超级页。

修改`kernel/vm.c`中`uvmcopy()`的逻辑，实现`fork()`时，如果父进程采用了超级页，则子进程同样使用超级页。

修改`kernel/vm.c`中`uvmunmap()`的逻辑，支持解除超级页的映射，以及将超级页降级为普通页。

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)


## 测试结果
以下是`make grade`的测试结果
```bash
== Test pgtbltest ==
$ make qemu-gdb
(5.1s)
== Test   pgtbltest: ugetpid ==
  pgtbltest: ugetpid: OK
== Test   pgtbltest: print_kpgtbl ==
  pgtbltest: print_kpgtbl: OK
== Test   pgtbltest: superpg ==
  pgtbltest: superpg: OK
== Test answers-pgtbl.txt ==
answers-pgtbl.txt: OK
== Test usertests ==
$ make qemu-gdb
(57.5s)
== Test time ==
time: OK
Score: 41/41
```