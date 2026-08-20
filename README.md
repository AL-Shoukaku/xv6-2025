# MIT 6.1810 Lab 6: Networking

该分支是我在 MIT 6.1810 实验中 Lab 6 的实验代码。

---

## 实验概览

本实验使用 QEMU 模拟出的 E1000 网卡设备和以太网（LAN），实现了设备的**驱动程序**，并在此基础上实现了一个简单的 **UDP 协议栈**。

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

### 1. Part One: NIC (moderate)

**涉及文件：`kernel/e1000.c`**

这一部分要完成 `kernel/e1000.c` 中的 `e1000_transmit()` 和 `e1000_recv()` 这两个函数，它们是 E1000 设备驱动程序的核心代码，分别负责向缓冲区发送分组和从缓冲区中接收分组。

#### 测试方法

##### e1000_transmit()

在一个终端 A 中运行 `python3 nettest.py txone`，然后在另一个终端 B 中运行 xv6 并执行 `nettest txone` 来发送一个分组。此时终端 A 会有以下输出，代表测试成功：

```bash
tx: listening for a UDP packet
txone: OK
```

此时运行 `tcpdump -XXnr packets.pcap` 可以看到以下输出：

```bash
reading from file packets.pcap, link-type EN10MB (Ethernet), snapshot length 65536
13:06:55.648657 IP 10.0.2.15.2003 > 10.0.2.2.26099: UDP, length 5
        0x0000:  5255 0a00 0202 5254 0012 3456 0800 4500  RU....RT..4V..E.
        0x0010:  0021 0000 0000 6411 3ebc 0a00 020f 0a00  .!....d.>.......
        0x0020:  0202 07d3 65f3 000d 0000 7478 6f6e 65    ....e.....txone
```

##### e1000_recv()

在一个终端 A 里启动 xv6，然后在另一个终端 B 中运行 `python3 nettest.py rxone`，此时 xv6 中出现以下字样代表测试成功：

```bash
ip_rx: received an IP packet
```

此时运行 `tcpdump -XXnr packets.pcap` 的输出如下：

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

**涉及文件：`kernel/net.c`**

这一部分实现了 `sys_bind()`、`sys_recv()` 和 `ip_rx()` 这三个函数，它们分别负责**端口号的绑定**、**分组的接收**以及**分组的入队出队**。

函数的具体功能如下：
- `sys_bind()`：系统调用 `bind(short port)` 的核心实现函数，用于**设置并初始化一个要监听的端口号** `port`，以便后续的 `recv(port, ...)` 可以从该端口接收分组。
- `ip_rx(char *buf, int len)`：负责识别分组是否为 UDP 分组，对应端口号是否已经绑定，以及对应的分组队列是否已满（最多 16 个）。**不满足条件的分组全部抛弃**。满足条件的则将缓冲区 `buf` 中的负载数据入队到对应端口号的分组队列中。
- `sys_recv()`：系统调用 `recv(short dport, int *src, short *sport, char *buf, int maxlen)` 的核心实现函数，用于从 `dport` 对应端口的接收分组队列中取出**到达时间最早的 UDP 分组**，将其源 IP、源端口号分别复制到 `*src` 和 `*sport` 中，并将分组的负载部分复制到 `buf` 中，最多复制 `maxlen` 字节。该系统调用在成功时返回复制的字节数，失败时返回 -1。注意 `src`、`sport` 以及 `buf` 都是**用户空间的虚拟地址**。此外，如果 `dport` 对应的接收分组队列为空，则该系统调用会**阻塞**，直到有新的分组到达。

此外还在 `kernel/net.c` 中维护了两个数据结构：
- `struct udp_port`：用于维护一个端口号的绑定状态以及对应的接收分组队列。
- `struct packet`：用于维护一个分组。

#### 测试方法

先在窗口 A 中运行 `python3 nettest.py grade`，然后在窗口 B 中运行 xv6 并执行 `nettest grade`，此时窗口 B 会输出测试结果：

```bash
txone: sending one packet
arp_rx: received an ARP packet
ip_rx: received an IP packet
ping0: starting
ping0: OK
ping1: starting
ping1: OK
ping2: starting
ping2: OK
ping3: starting
ping3: OK
dns: starting
DNS arecord for pdos.csail.mit.edu. is 128.52.129.126
dns: OK
free: OK
```

---

## 参考资料

- [MIT 6.1810 课程主页](https://pdos.csail.mit.edu/6.1810/2025/index.html)
- [xv6 指导书](https://pdos.csail.mit.edu/6.1810/2025/xv6/book-riscv-rev5.pdf)
- [Software Developer's Manual](https://pdos.csail.mit.edu/6.1810/2025/readings/8254x_GBe_SDM.pdf)

## 测试结果
以下是 `make grade` 的测试结果：

```bash
== Test running nettest ==
$ make qemu-gdb
(19.1s)
== Test   nettest: txone ==
  nettest: txone: OK
== Test   nettest: arp_rx ==
  nettest: arp_rx: OK
== Test   nettest: ip_rx ==
  nettest: ip_rx: OK
== Test   nettest: ping0 ==
  nettest: ping0: OK
== Test   nettest: ping1 ==
  nettest: ping1: OK
== Test   nettest: ping2 ==
  nettest: ping2: OK
== Test   nettest: ping3 ==
  nettest: ping3: OK
== Test   nettest: dns ==
  nettest: dns: OK
== Test   nettest: free ==
  nettest: free: OK
== Test time ==
time: OK
Score: 171/171
```