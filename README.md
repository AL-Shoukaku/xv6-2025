# 2025 MIT 6.1810 Lab

--- 

## 项目介绍

MIT 6.1810 是麻省理工学院开设的一门**操作系统**公开课，以其基于 xv6 的实验项目而闻名。

2025 年的课程总共有 9 个实验，涵盖了页表，锁，文件系统等操作系统的核心概念。每个实验要求在已有的 xv6 代码框架下实现新的功能和特性，在实践中深入对操作系统的理解。

本仓库是我对全部 9 个实验的实现和总结，均已通过自动化测试。

## 环境配置

- **主机系统**：Windows 11 + WSL2（Ubuntu 24.04）

通过以下命令安装必要的依赖：

```bash
$ sudo apt-get update && sudo apt-get upgrade
$ sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

其它系统的环境配置请参考[官方工具指南](https://pdos.csail.mit.edu/6.1810/2025/tools.html)

## 快速开始

```bash
# 克隆仓库
git clone git@github.com:AL-Shoukaku/xv6-2025.git
# 进入仓库目录
cd xv6-2025
# 查看分支
git branch -a
# 切换到对应实验分支
git checkout util
# 运行评测
make grade
# 运行 xv6
make qemu
```

---

## Lab 一览

### Lab 1: Xv6 and Unix Utilities

lab1 主要是熟悉 xv6 的启动方式并**实现了 5 个用户程序**，包括 `sleep`,`sixfive`,`memdump`,`find` 和 `find -exec`.

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/util)
- [技术博客](https://al-shoukaku.github.io/2026/08/23/MIT6.1810-lab1/)

### Lab 2: Syscall

lab2 的主题是**系统调用**，包括使用 gdb 调试内核，实现 `sandbox` 系统调用以及攻击 xv6 的内核漏洞。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/syscall)
- [技术博客](https://al-shoukaku.github.io/2026/09/01/MIT6.1810-lab2/)

### Lab 3: Page Tables

lab3 的主题是**页表**，包括分析与打印进程的页表信息，通过共享页面来优化 `getpid` 系统调用的性能，以及在 xv6 中实现超级页(superpage)。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/pgtbl)

### Lab 4: Traps

lab4 的主题是**陷阱处理**，包括阅读 RISC-v 汇编代码，解析内核栈来打印 backtrace 以及实现定时调用指定函数的 `sigalarm` 和 `sigreturn` 两个系统调用。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/traps)

### Lab 5: Copy-on-Write Fork for xv6

lab5 的主题是**写时复制（Copy-on-Write）**，综合前面的页表，陷入等机制，在 xv6 中实现写时复制机制。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/cow)

### Lab 6: Networking

lab6 的主题是**设备驱动**，包括完善 e1000 网卡的驱动程序以及实现一个简单的 UDP 协议栈。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/net)

### Lab 7: Locks

lab7 的主题是**锁**，包括优化 xv6 的内存分配器来减少锁的竞争，以及使用原子操作来实现一个读写锁(写者优先)。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/lock)

### Lab 8: file system

lab8 的主题是**文件系统**，包括修改 inode 索引结构来支持大文件，以及实现软链接功能。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/fs)

### Lab 9: mmap

lab9 的主题是**内存映射（mmap）**，包括实现 `mmap` 系统调用来将文件映射到用户地址空间并实现懒加载，以及实现 `munmap` 系统调用来解除映射。

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/mmap)

---

## 参考资料

- [2025 MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6 指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)
- [xv6 源代码](https://github.com/mit-pdos/xv6-riscv)
