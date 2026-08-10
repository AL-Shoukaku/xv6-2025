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

这一部分要完成`kernel/e1000.c`中的`e1000_transmit()`和`e1000_recv()`这两个函数，它们是设备 e1000 驱动器的核心代码，分别负责向缓冲区发送分组和从缓冲区中接收分组。

#### 测试方法

##### e1000_transmit()

在一个终端 A 中运行`python3 nettest.py txone`,然后在另一个终端 B 中运行 xv6 并执行`nettest txone`来发送一个分组。此时终端 A 会有以下输出，代表测试成功：

```bash
tx: listening for a UDP packet
txone: OK
```

此时运行`tcpdump -XXnr packets.pcap`可以看到以下输出：

```bash
reading from file packets.pcap, link-type EN10MB (Ethernet), snapshot length 65536
13:06:55.648657 IP 10.0.2.15.2003 > 10.0.2.2.26099: UDP, length 5
        0x0000:  5255 0a00 0202 5254 0012 3456 0800 4500  RU....RT..4V..E.
        0x0010:  0021 0000 0000 6411 3ebc 0a00 020f 0a00  .!....d.>.......
        0x0020:  0202 07d3 65f3 000d 0000 7478 6f6e 65    ....e.....txone
```

##### e1000_recv()

在一个终端 A 里启动 xv6,然后在另一个终端 B 中运行`python3 nettest.py rxone`,此时 xv6 中出现以下字样代表测试成功：

```bash
ip_rx: received an IP packet
```

此时运行`tcpdump -XXnr packets.pcap`的输出如下：

```bash
reading from file packets.pcap, link-type EN10MB (Ethernet), snapshot length 65536
13:09:53.741395 ARP, Request who-has 10.0.2.15 tell 10.0.2.2, length 46
        0x0000:  ffff ffff ffff 5255 0a00 0202 0806 0001  ......RU........
        0x0010:  0800 0604 0001 5255 0a00 0202 0a00 0202  ......RU........
        0x0020:  0000 0000 0000 0a00 020f 0000 0000 0000  ................
        0x0030:  0000 0000 0000 0000 0000 0000            ............
13:09:53.743486 ARP, Reply 10.0.2.15 is-at 52:54:00:12:34:56, length 28
        0x0000:  5255 0a00 0202 5254 0012 3456 0806 0001  RU....RT..4V....
        0x0010:  0800 0604 0002 5254 0012 3456 0a00 020f  ......RT..4V....
        0x0020:  5255 0a00 0202 0a00 0202                 RU........
13:09:53.816269 IP 10.0.2.2.32787 > 10.0.2.15.2000: UDP, length 3
        0x0000:  5254 0012 3456 5255 0a00 0202 0800 4500  RT..4VRU......E.
        0x0010:  001f 0000 0000 4011 62be 0a00 0202 0a00  ......@.b.......
        0x0020:  020f 8013 07d0 000b 6d6a 7879 7a00 0000  ........mjxyz...
        0x0030:  0000 0000 0000 0000 0000 0000            ............
```

### 2. Part Two: UDP Receive (moderate)

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)
- [Software Developer's Manual](https://pdos.csail.mit.edu/6.1810/2025/readings/8254x_GBe_SDM.pdf)

## 测试结果
以下是`make grade`的测试结果
```bash

```