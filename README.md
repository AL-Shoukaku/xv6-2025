# MIT 6.1810 Lab 9: mmap

该分支是我在MIT6.1810实验中 lab9 的实验代码.

---

## 实验概览

本次实验在 xv6 中实现了`mmap()`和`munmap()`两个系统调用，支持将文件映射到进程的地址空间中，并在需要时解除映射。同时，实现了对映射区域的懒加载和权限控制。

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

### 1. mmap (hard)

**核心文件:`kernel/sysfile.c,kernel/vm.c,kernel/proc.c`**

#### 核心功能

实现了`mmap(void *addr, uint64 len, int prot, int flags,int fd, uint64 offset)`系统调用，支持以下功能:

- 核心功能为将文件`fd`映射到当前进程的地址空间中，会返回映射的起实地址
- `offset`为文件中开始映射的偏移量，`len`为映射的字节数，不要求与文件大小相同。
- `prot`为映射区域的权限，支持`PROT_NONE`, `PROT_READ`, `PROT_WRITE`, `PROT_EXEC`，可以组合使用。
- `flags`为映射区域的类型，支持`MAP_SHARED`, `MAP_PRIVATE`。前者在对映射区域进行修改后，会在`munmap`或退出时将修改写回文件，后者则不会写回文件，且后者能打开只读模式的文件。
- `addr`默认为 0，内核会选择一个合适的地址进行映射，具体来讲是从`TRAPFRAME`地址向下进行生长。

实现了`munmap(void *addr, uint64 len)`系统调用，支持以下功能:

- 核心功能为将`mmap()`j建立的映射区域从当前进程的地址空间中解除映射。
- `addr`为开始解除的地址，`len`为解除的字节数。
- 要求解除的区域要么等于整个映射区域，要么是从头开始的连续区域，要么是连续到尾的区域，**禁止在中间解除映射**

#### 其它改动

在`kernel/proc.h`中增加了`struct VMA`数据结构，用来记录一个`mmap()`映射的相关信息。

在`struct proc`中新增字段`vma[]`和`vma_sz`用来记录`mmap()`的映射区域。

修改了`kernel/vm.c`中的页错误处理函数`vmfault()`，支持对`mmap()`映射区域的懒加载。

修改了`kernel/proc.c`中的`kfork()`和`kexit()`函数，支持对`mmap()`映射区域的复制和释放。

#### 测试方法

在 xv6 中运行`mmaptest`进行测试，正确的输出结果如下：

```bash
test basic mmap
test basic mmap: OK
test mmap private
test mmap private: OK
test mmap read-only
test mmap read-only: OK
test mmap read/write
test mmap read/write: OK
test mmap dirty
test mmap dirty: OK
test not-mapped unmap
test not-mapped unmap: OK
test lazy access
test lazy access: OK
test mmap two files
test mmap two files: OK
test fork
test fork: OK
test munmap prevents access
usertrap(): unexpected scause 0xd pid=5
            sepc=0xa2a stval=0x3ffffee000
usertrap(): unexpected scause 0xd pid=6
            sepc=0xab2 stval=0x3ffffed000
test munmap prevents access: OK
test writes to read-only mapped memory
usertrap(): unexpected scause 0xf pid=7
            sepc=0xbfa stval=0x3fffffc000
test writes to read-only mapped memory: OK
mmaptest: all tests succeeded
```

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)

## 测试结果
以下是`make grade`的测试结果
```bash
== Test running mmaptest ==
$ make qemu-gdb
(4.4s)
== Test   mmaptest: mmap basic ==
  mmaptest: mmap basic: OK
== Test   mmaptest: mmap private ==
  mmaptest: mmap private: OK
== Test   mmaptest: mmap read-only ==
  mmaptest: mmap read-only: OK
== Test   mmaptest: mmap read/write ==
  mmaptest: mmap read/write: OK
== Test   mmaptest: mmap dirty ==
  mmaptest: mmap dirty: OK
== Test   mmaptest: not-mapped unmap ==
  mmaptest: not-mapped unmap: OK
== Test   mmaptest: lazy access ==
  mmaptest: lazy access: OK
== Test   mmaptest: two files ==
  mmaptest: two files: OK
== Test   mmaptest: fork_test ==
  mmaptest: fork_test: OK
== Test   mmaptest: munmap_noaccess ==
  mmaptest: munmap_noaccess: OK
== Test   mmaptest: read_only_write ==
  mmaptest: read_only_write: OK
== Test usertests ==
$ make qemu-gdb
usertests: OK (70.5s)
== Test time ==
time: OK
Score: 170/170
```