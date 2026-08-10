# MIT 6.1810 Lab 6: networking

该分支是我在MIT6.1810实验中 lab6 的实验代码.

---

## 实验概览


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

### 1. Part One: NIC (moderate)

**涉及文件:`kernel/e1000.c`**



---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)
- [Software Developer's Manual](https://pdos.csail.mit.edu/6.1810/2025/readings/8254x_GBe_SDM.pdf)

## 测试结果
以下是`make grade`的测试结果
```bash

```