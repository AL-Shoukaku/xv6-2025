# MIT 6.1810 Lab 4: traps

该分支是我在MIT6.1810实验中lab4的实验代码.

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

### 1. RISC-V assembly (easy)

通过运行 `make fs.img`，我们会将`user/call.c`编译为RISC-V汇编代码，在`user/call.asm`中查看。

我们需要基于这些汇编代码回答任务中的若干问题，答案在`answers-traps.txt`中。

要回答的问题：

> Which registers contain arguments to functions? For example, which register holds 13 in main's call to `printf`?

> Where is the call to function `f` in the assembly code for main? Where is the call to `g`? (Hint: the compiler may inline functions.)

> At what address is the function `printf` located?

> What value is in the register `ra` just after the `jalr` to `printf` in `main`?

> Run the following code.
>
> ```bash
> unsigned int i = 0x00646c72;
> printf("H%x Wo%s", 57616, (char *) &i);
> ```
>
> What is the output? Here's an ASCII table that maps bytes to characters.
> The output depends on that fact that the RISC-V is little-endian. If the RISC-V were instead big-endian what would you set `i` to in order to yield the same output? Would you need to change `57616` to a different value?
> Here's a description of little- and big-endian and a more whimsical description.

> In the following code, what is going to be printed after `'y='` ? (note: the answer is not a specific value.) Why does this happen?
>
> ```bash
> printf("x=%d y=%d", 3);
> ```

### 2. Backtrace (moderate)

**涉及文件：`kernel/printf.c`**

#### 具体功能

在`kernel/printf.c`中实现`backtrace()`函数，该函数使用帧指针遍历内核栈，通过打印每一个函数的返回地址来反应当前的**函数调用关系**。

该函数插入到了`sys_pause()`,而`bttest`程序会调用`sys_pause`,其输出如下：

```bash
backtrace:
0x0000000080001e9c
0x0000000080001d18
0x0000000080001a9c
```

#### 测试方法

拿到`bttest`输出的地址后，退出 xv6 ，在终端输入`addr2line -e kernel/kernel`,然后把刚刚的地址逐个输入进去，可以得到：

```bash
project_path/xv6-labs-2025/kernel/sysproc.c:85
project_path/xv6-labs-2025/kernel/syscall.c:141 (discriminator 1)
project_path/xv6-labs-2025/kernel/trap.c:80
```

### 3. Alarm (hard)



---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)


## 测试结果
以下是`make grade`的测试结果
```bash

```