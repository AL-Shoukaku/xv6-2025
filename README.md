# MIT 6.1810 Lab 7: locks
该分支是我在MIT6.1810实验中 lab7 的实验代码.

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

### 1. Memory allocator (moderate)

**涉及文件:`kernel/kalloc.c`**

#### 核心功能

通过重新设计 xv6 的内存分配器，减少锁的竞争来提高性能。

将原来的全局共享一个空闲链表，改为每一个 CPU 各自维护自己独立的空闲链表，这样每个 CPU 都可以独立地进行内存分配和释放操作，从而减少锁的竞争。

当一个 CPU 的空闲链表耗尽时，应当允许它从其他 CPU 的空闲链表中获取空闲页面。

#### 测试方法

在 xv6 中运行`kalloctest`来进行测试，最终的输出结果如下：

```bash
start test1
test1 results:
--- lock kmem stats
lock: kmem: #test-and-set 0 #acquire() 80059
lock: kmem: #test-and-set 0 #acquire() 119299
lock: kmem: #test-and-set 0 #acquire() 113781
lock: kmem: #test-and-set 0 #acquire() 120054
lock: kmem: #test-and-set 0 #acquire() 64
lock: kmem: #test-and-set 0 #acquire() 64
lock: kmem: #test-and-set 0 #acquire() 64
lock: kmem: #test-and-set 0 #acquire() 64
--- top 5 contended locks:
lock: virtio_disk: #test-and-set 183985 #acquire() 180
lock: proc: #test-and-set 180750 #acquire() 216512
lock: proc: #test-and-set 74317 #acquire() 216565
lock: proc: #test-and-set 63345 #acquire() 616771
lock: proc: #test-and-set 56024 #acquire() 616775
tot= 0
test1 OK
start test2
total free number of pages: 32463 (out of 32768)
..........
test2 OK
start test3
..........child done 10000

test3 OK
start test4
............................child done 100000
.child done 100000
.child done 100000
--- lock kmem stats
lock: kmem: #test-and-set 7282 #acquire() 693336
lock: kmem: #test-and-set 10321 #acquire() 777890
lock: kmem: #test-and-set 10509 #acquire() 746934
lock: kmem: #test-and-set 18694 #acquire() 1271193
lock: kmem: #test-and-set 21 #acquire() 119465
lock: kmem: #test-and-set 279 #acquire() 119465
lock: kmem: #test-and-set 8 #acquire() 119465
lock: kmem: #test-and-set 21 #acquire() 119465
--- top 5 contended locks:
lock: wait_lock: #test-and-set 73421672 #acquire() 40031
lock: proc: #test-and-set 15664568 #acquire() 924105
lock: proc: #test-and-set 8740896 #acquire() 2290543
lock: proc: #test-and-set 439230 #acquire() 1794296
lock: proc: #test-and-set 292984 #acquire() 1458723
tot= 47135

test4 OK
```

这里面`test 1`和`test 4`会统计`acquire()`的次数，注意这个测试中电脑的工作负载会显著影响统计值，因此测试时**尽量把电脑里的其他程序关闭**。

`test 2`和`test 3`则负责测试窃取时的正确性，检查是否因窃取丢失页面。

运行`usertests sbrkmuch`来测试我们的系统是否仍然能申请大量内存，结果如下：

```bash
usertests starting
test sbrkmuch: OK
ALL TESTS PASSED
```

### 2. Read-write lock (moderate)

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)

## 测试结果
以下是`make grade`的测试结果
```bash

```