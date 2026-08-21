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

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/util)

### Lab 2: Syscall

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/syscall)

### Lab 3: Page Tables

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/pgtbl)

### Lab 4: Traps

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/traps)

### Lab 5: Copy-on-Write Fork for xv6

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/cow)

### Lab 6: Networking

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/net)

### Lab 7: Locks

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/lock)

### Lab 8: file system

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/fs)

### Lab 9: mmap

- [分支代码](https://github.com/AL-Shoukaku/xv6-2025/tree/mmap)

---

## 参考资料

- [2025 MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)

