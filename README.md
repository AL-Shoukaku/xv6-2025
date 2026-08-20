# MIT 6.1810 Lab 5: Copy-on-Write Fork for xv6

该分支是我在 MIT 6.1810 实验中 Lab 5 的实验代码。

---

## 实验概览

本次实验的主题是**写时复制（COW）**。在实验中，我们将在 xv6 中亲手实现写时复制功能，通过在 `fork()` 过程中共享内存页，并在写入时才进行复制，成功减少了内存占用并提升了性能。

可以在 `task.md` 中查看整个实验内容。

---
## 开发环境
- **主机系统**：Windows 11 + WSL2（Ubuntu 24.04）
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

### 1. Implement copy-on-write fork (hard)

**核心文件：`kernel/vm.c`、`kernel/kalloc.c`**

#### 详细内容

- 利用页表项中权限的**保留位**，新增了一个标记位 `PTE_COW`，用于标记该页是否为写时复制页。
- 在 `kernel/kalloc.c` 中维护全局数组 `reference`，以物理页号为索引，记录物理页的引用次数。
- 修改 `kernel/vm.c` 中的 `uvmcopy()` 函数，不再复制内存，而是在父子进程间建立共享页面并设置好写时复制权限。
- 修改 `kernel/vm.c` 中的 `vmfault()` 和 `copyout()` 函数，实现**页错误**时对于写时复制页的识别，并进行复制处理。

#### 测试方法

在 xv6 中运行 `cowtest` 可以进行写时复制功能的测试，输出结果如下：

```bash
simple: ok
simple: ok
three: ok
three: ok
three: ok
file: ok
forkfork: ok
ALL COW TESTS PASSED
```

还可以运行 `usertests -q` 来验证新增功能是否对原有功能造成影响，看到以下字样即为成功：

```bash
ALL TESTS PASSED
```

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6 指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)


## 测试结果
以下是 `make grade` 的测试结果：

```bash
== Test running cowtest ==
$ make qemu-gdb
(29.8s)
== Test   simple ==
  simple: OK
== Test   three ==
  three: OK
== Test   file ==
  file: OK
== Test   forkfork ==
  forkfork: OK
== Test usertests ==
$ make qemu-gdb
(65.6s)
== Test   usertests: copyin ==
  usertests: copyin: OK
== Test   usertests: copyout ==
  usertests: copyout: OK
== Test   usertests: all tests ==
  usertests: all tests: OK
== Test time ==
time: OK
Score: 130/130
```
