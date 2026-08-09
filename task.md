# Lab: networking

In this lab you will write an xv6 device driver for a network interface card (NIC), and then write the receive half of an ethernet/IP/UDP protocol processing stack.

Fetch the xv6 source for the lab and check out the `net` branch:

```bash
$ git fetch
$ git checkout net
$ make clean
```

## Background

> Before writing code, you may find it helpful to review "Chapter 6: Interrupts and device drivers" in the xv6 book.

You'll use a network device called the E1000 to handle network communication. To xv6 (and the driver you write), the E1000 looks like a real piece of hardware connected to a real Ethernet local area network (LAN). In fact, the E1000 your driver will talk to is an emulation provided by qemu, connected to a LAN that is also emulated by qemu. On this emulated LAN, xv6 (the "guest") has an IP address of 10.0.2.15. Qemu arranges for the computer running qemu (the "host") to appear on the LAN with IP address 10.0.2.2. When xv6 uses the E1000 to send a packet to 10.0.2.2, qemu delivers the packet to the appropriate application on the host.

You will use QEMU's "user-mode network stack". QEMU's documentation has more about the user-mode stack here. We've updated the Makefile to enable QEMU's user-mode network stack and E1000 network card emulation.

The Makefile configures QEMU to record all incoming and outgoing packets to the file `packets.pcap` in your lab directory. It may be helpful to review these recordings to confirm that xv6 is transmitting and receiving the packets you expect. To display the recorded packets:

```bash
tcpdump -XXnr packets.pcap
```

We've added some files to the xv6 repository for this lab. The file `kernel/e1000.c` contains initialization code for the E1000 as well as empty functions for transmitting and receiving packets, which you'll fill in.`kernel/e1000_dev.h` contains definitions for registers and flag bits defined by the E1000 and described in the Intel E1000 Software Developer's Manual.`kernel/net.c` and `kernel/net.h` contain a simple network stack that implements the IP, UDP, and ARP protocols;`net.c` has complete code for user processes to send UDP packets, but lacks most of the code to receive packets and deliver them to user space. Finally,`kernel/pci.c` contains code that searches for an E1000 card on the PCI bus when xv6 boots.

## Part One: NIC ( moderate )

Your job is to complete `e1000_transmit()` and `e1000_recv()`, both in `kernel/e1000.c`, so that the driver can transmit and receive packets. You are done with this part when `make grade` says your solution passes the "txone" and "rxone" tests.

> While writing your code, you'll refer to the E1000 Software Developer's Manual, particularly:
>
> - Section 3.2 describes packet reception (but skip 3.2.8 and 3.2.9).
> - Sections 3.3.1, 3.3.2, 3.3.3, and 3.4 describe transmission.
> - Section 13 describes the E1000's registers (for reference as needed; don't read the whole thing).

The Software Developer's Manual describes several closely related Ethernet controllers. QEMU emulates the 82540EM. You'll need to be familiar with the sections of Chapter 3 noted above. The other chapters mostly cover aspects of the E1000 that your driver won't have to interact with. Don't worry about the details at first; just get a feel for how the document is structured so you can find things later. The E1000 has many advanced features, most of which you can ignore. Only a small set of basic features is needed to complete this lab.

The `e1000_init()` function we provide you in `e1000.c` configures the E1000 to read packets to be transmitted from RAM, and to write received packets to RAM. This technique is called DMA, for direct memory access, referring to the fact that the E1000 hardware directly writes and reads packets to/from RAM.

Because bursts of packets might arrive faster than the driver can process them,`e1000_init()` provides the E1000 with multiple buffers into which the E1000 can write received packets. The E1000 requires these buffers to be described by an array of "descriptors" in RAM; each descriptor contains an address in RAM where the E1000 can write a received packet.`struct rx_desc` describes the descriptor format. The array of descriptors is called the receive ring, or receive queue. It's a circular ring in the sense that when the card or driver reaches the end of the array, it wraps back to the beginning.`e1000_init()` allocates packet buffers with `kalloc()` for the E1000 to DMA into. There is also a transmit ring into which the driver should place packets it wants the E1000 to send.`e1000_init()` configures the two rings to have size `RX_RING_SIZE` and `TX_RING_SIZE`.

When the network stack in `net.c` needs to send a packet, it calls `e1000_transmit()` with a pointer to a buffer that holds the packet to be sent; `net.c` allocates this buffer with `kalloc()`. Your transmit code must place a pointer to the packet data in a descriptor in the TX (transmit) ring. `struct tx_desc` describes the descriptor format. You will need to ensure that each buffer is eventually passed to `kfree()`, but only after the E1000 has finished transmitting the packet (the E1000 sets the `E1000_TXD_STAT_DD` bit in the descriptor to indicate this).

When the E1000 receives each packet from the ethernet, it DMAs the packet to the memory pointed to by `addr` in the next RX (receive) ring descriptor. If an E1000 interrupt is not already pending, the E1000 asks the PLIC to deliver one as soon as interrupts are enabled. Your `e1000_recv()` code must scan the RX ring and deliver each new packet to the network stack (in `net.c`) by calling `net_rx()`. You will then need to allocate a new buffer and place it into the descriptor, so that when the E1000 reaches that point in the RX ring again it finds a fresh buffer into which to DMA a new packet.

In addition to reading and writing the descriptor rings in RAM, your driver will need to interact with the E1000 through its memory-mapped control registers, to detect when received packets are available and to inform the E1000 that the driver has filled in some TX descriptors with packets to send. The global variable `regs` holds a pointer to the E1000's first control register; your driver can get at the other registers by indexing `regs` as an array. You'll need to use indices `E1000_RDT` and `E1000_TDT` in particular.

To test e1000_transmit() sending a single packet, run `python3 nettest.py txone` in one window, and in another window run `make qemu` and then run `nettest txone` in xv6, which sends a single packet. `nettest.py` will print `txone: OK` if all went well (i.e. qemu's e1000 emulator saw the packet on the DMA ring and forwarded it outside of qemu).

If transmitting worked, `tcpdump -XXnr packets.pcap` shold produce output like this:

```bash
reading from file packets.pcap, link-type EN10MB (Ethernet)
21:27:31.688123 IP 10.0.2.15.2000 > 10.0.2.2.25603: UDP, length 5
        0x0000:  5255 0a00 0202 5254 0012 3456 0800 4500  RU....RT..4V..E.
        0x0010:  0021 0000 0000 6411 3ebc 0a00 020f 0a00  .!....d.>.......
        0x0020:  0202 07d0 6403 000d 0000 7478 6f6e 65    ....d.....txone
```

To test e1000_recv() receiving two packets (an ARP query, then a IP/UDP packet), run `make qemu` in one window, and `python3 nettest.py rxone` in another window. `nettest.py rxone` sends a single UDP packet via qemu to xv6; qemu actually first sends an ARP request to xv6, and (after xv6 returns an ARP reply) qemu forwards the UDP packet to xv6. If e1000_recv() works correctly and passes those packets to `net_rx()`, `net.c` should print

```bash
arp_rx: received an ARP packet
ip_rx: received an IP packet
```

`net.c` already contains the code to detect qemu's ARP request and call `e1000_transmit()` to send its reply. This test requires that both e1000_transmit() and e1000_recv() work. In addition, if all went well, `tcpdump -XXnr packets.pcap` should produce output like this:

```bash
reading from file packets.pcap, link-type EN10MB (Ethernet)
21:29:16.893600 ARP, Request who-has 10.0.2.15 tell 10.0.2.2, length 28
        0x0000:  ffff ffff ffff 5255 0a00 0202 0806 0001  ......RU........
        0x0010:  0800 0604 0001 5255 0a00 0202 0a00 0202  ......RU........
        0x0020:  0000 0000 0000 0a00 020f                 ..........
21:29:16.894543 ARP, Reply 10.0.2.15 is-at 52:54:00:12:34:56, length 28
        0x0000:  5255 0a00 0202 5254 0012 3456 0806 0001  RU....RT..4V....
        0x0010:  0800 0604 0002 5254 0012 3456 0a00 020f  ......RT..4V....
        0x0020:  5255 0a00 0202 0a00 0202                 RU........
21:29:16.902656 IP 10.0.2.2.61350 > 10.0.2.15.2000: UDP, length 3
        0x0000:  5254 0012 3456 5255 0a00 0202 0800 4500  RT..4VRU......E.
        0x0010:  001f 0000 0000 4011 62be 0a00 0202 0a00  ......@.b.......
        0x0020:  020f efa6 07d0 000b fdd6 7879 7a         ..........xyz
```

Your output will look somewhat different, but it should contain the strings "ARP, Request", "ARP, Reply", "UDP", and "....xyz".

If both of the above tests work, then `make grade` should show that the first two tests pass.

## e1000 hints

For `e1000_transmit`:

- Start by adding print statements to `e1000_transmit()` and `e1000_recv()`, and running (in xv6) `nettest txone`. You should see from your print statements that `nettest txone` generates a call to `e1000_transmit`.
- The descriptor definitions in `e1000_dev.h` use the "legacy" transmit descriptor format (Section 3.3.3).
- First ask the E1000 for the TX ring index at which it's expecting the next packet, by reading the `E1000_TDT` control register.
- Then check if the the ring is overflowing. If `E1000_TXD_STAT_DD` is not set in the descriptor indexed by `E1000_TDT`, the E1000 hasn't finished the corresponding previous transmission request, so return an error.
- Otherwise, use `kfree()` to free the last buffer that was transmitted from that descriptor (if there was one).
- Then fill in the descriptor. Set the necessary cmd flags (look at Section 3.3 in the E1000 manual).
- Finally, update the ring position by adding one to `E1000_TDT` modulo `TX_RING_SIZE`.

For `e1000_recv`:

- First ask the E1000 for the ring index at which the next waiting received packet (if any) is located, by fetching the `E1000_RDT` control register and adding one modulo `RX_RING_SIZE`.
- Then check if a new packet is available by checking for the
`E1000_RXD_STAT_DD` bit in the `status` portion of the descriptor. If not, stop.
- Deliver the packet buffer to the network stack by calling `net_rx()`.
- Then allocate a new buffer using `kalloc()` to replace the one just given to `net_rx()`. Clear the descriptor's status bits to zero.
- Finally, update the `E1000_RDT` register to be the index of the last ring descriptor processed.
- `e1000_init()` initializes the RX ring with buffers, and you'll want to look at how it does that and perhaps borrow code.
- At some point the total number of packets that have ever arrived will exceed the ring size (16); make sure your code can handle that.
- The e1000 can deliver more than one packet per interrupt; your `e1000_recv` should handle that situation.

You'll need locks to cope with the possibility that xv6 might use the E1000 from more than one process, or might be using the E1000 in a kernel thread when an interrupt arrives.

## Part Two: UDP Receive ( moderate )

UDP, the User Datagram Protocol, allows user processes on different Internet hosts to exchange individual packets (datagrams). UDP is layered on top of IP. A user process indicates which host it wants to send a packet to by specifying a 32-bit IP address. Each UDP packet contains a source port number and a destination port number; processes can request to receive packets that arrive addressed to particular port numbers, and can specify the destination port number when sending. Thus two processes on different hosts can communicate with UDP if they know each others' IP addresses and the port numbers each is listening for. For example, Google operates a DNS name server on the host with IP address 8.8.8.8, listening on UDP port 53.

In this task, you'll add code to `kernel/net.c` to receive UDP packets, queue them, and allow user processes to read them.`net.c` already contains the code required for user processes to transmit UDP packets (with the exception of e1000_transmit(), which you provide).

> Your job is to fill in the implementations of `ip_rx()`, `sys_recv()`, and `sys_bind()` in `kernel/net.c`. You are done when `make grade` says your solution passes all of the tests.
>
> You can run the same tests that `make grade` runs by starting `python3 nettest.py grade` in one window, and then running `nettest grade` inside xv6 in another window. If all goes well, you should see this in the first (outside of xv6) window:
>
> ```bash
> $ python3 nettest.py grade
> txone: OK
> rxone: sending one UDP packet
> ```
>
> and this in the xv6 window:
>
> ```bash
> $ nettest grade
> txone: sending one packet
> arp_rx: received an ARP packet
> ip_rx: received an IP packet
> ping0: starting
> ping0: OK
> ping1: starting
> ping1: OK
> ping2: starting
> ping2: OK
> ping3: starting
> ping3: OK
> dns: starting
> DNS arecord for pdos.csail.mit.edu. is 128.52.129.126
> dns: OK
> free: OK
> ```

This lab's system-call API specification for UDP looks like this:

- `send(short sport, int dst, short dport, char *buf, int len)`: This system call sends a UDP packet to the host with IP address `dst`, and (on that host) the process listening to port `dport`. The packet's source port number will be `sport` (this port number is reported to the receiving process, so that it can reply to the sender). The content ("payload") of the UDP packet will the `len` bytes at address `buf`. The return value is 0 on success, and -1 on failure.
- `recv(short dport, int *src, short *sport, char *buf, int maxlen)`: This system call returns the payload of a UDP packet that arrives with destination port `dport`. If one or more packets arrived before the call to `recv()`, it should return right away with the earliest waiting packet. If no packets are waiting, `recv()` should wait until a packet for `dport` arrives. `recv()` should see arriving packets for a given port in arrival order. `recv()` copies the packet's 32-bit source IP address to `*src`, copies the packet's 16-bit UDP source port number to `*sport`, copies at most `maxlen` bytes of the packet's UDP payload to `buf`, and removes the packet from the queue. The system call returns the number of bytes of the UDP payload copied, or -1 if there was an error.
- `bind(short port)`: A process should call `bind(port)` before it calls `recv(port, ...)`. If a UDP packet arrives with a destination port that hasn't been passed to `bind()`, `net.c` should discard that packet. The reason for this system call is to initialize any structures `net.c` needs in order to store arriving packets for a subsequent `recv()` call.
- `unbind(short port)`: You do not need to implement this system call, since the test code does not use it. But you can if you like in order to provide symmetry with `bind()`.

All the addresses and port numbers passed as arguments to these system calls, and returned by them, must be in host byte order (see below).

You'll need to provide the kernel implementations of the system calls, with the exception of `send()`. The program `user/nettest.c` uses this API.

To make `recv()` work, you'll need to add code to `ip_rx()`, which `net_rx()` calls for each received IP packet. `ip_rx()` should decide if the arriving packet is UDP, and whether its destination port has been passed to `bind()`; if both are true, it should save the packet where `recv()` can find it. However, for any given port, no more than 16 packets should be saved; if 16 are already waiting for `recv()`, an incoming packet for that port should be dropped. The point of this rule is to prevent a fast or abusive sender from forcing xv6 to run out of memory. Furthermore, if packets are being dropped for one port because it already has 16 packets waiting, that should not affect packets arriving for other ports.

The packet buffers that `ip_rx()` looks at contain a 14-byte ethernet header, followed by a 20-byte IP header, followed by an 8-byte UDP header, followed by the UDP payload. You'll find C struct definitions for each of these in `kernel/net.h`. Wikipedia has a description of the IP header here, and UDP here.

Production IP/UDP implementations are complex, handling protocol options and validating invariants. You only need to do enough to pass `make grade`. Your code needs to look at ip_p and ip_src in the IP header, and dport, sport, and ulen in the UDP header.

You will have to pay attention to byte order. Ethernet, IP, and UDP header fields that contain multi-byte integers place the most significant byte first in the packet. The RISC-V CPU, when it lays out a multi-byte integer in memory, places the least-significant byte first. This means that, when code extracts a multi-byte integer from a packet, it must re-arrange the bytes. This applies to short (2-byte) and int (4-byte) fields. You can use the `ntohs()` and `ntohl()` functions for 2-byte and 4-byte fields, respectively. Look at `net_rx()` for an example of this when looking at the 2-byte ethernet type field.

If there are errors or omissions in your E1000 code, they may only start to cause problems during the ping tests. For example, the ping tests send and receive enough packets that the descriptor ring indices will wrap around.

Some hints:

- Create a struct to keep track of bound ports and the packets in their queues.
- Refer to the `sleep(void *chan, struct spinlock *lk)` and `wakeup(void *chan)` functions in `kernel/proc.c` to implement the waiting logic for `recv()`.
- The destination addresses that `sys_recv()` copies the packets to are virtual addresses; you will have to copy from the kernel to the current user process.
- Make sure to free packets that have been copied over or have been dropped.

## Submit the lab

### Time spent

Create a new file,`time.txt`, and put in a single integer, the number of hours you spent on the lab. git add and git commit the file.

### Answers

If this lab had questions, write up your answers in `answers-*.txt`. git add and git commit these files.

### Submit

Assignment submissions are handled by Gradescope. You will need an MIT gradescope account. See Piazza for the entry code to join the class. Use this link if you need more help joining.

When you're ready to submit, run make zipball, which will generate `lab.zip`. Upload this zip file to the corresponding Gradescope assignment.

If you run make zipball and you have either uncomitted changes or untracked files, you will see output similar to the following:

```bash
 M hello.c
?? bar.c
?? foo.pyc
Untracked files will not be handed in.  Continue? [y/N]
```

Inspect the above lines and make sure all files that your lab solution needs are tracked, i.e., not listed in a line that begins with `??`. You can cause `git` to track a new file that you create using git add {filename}.

- Please run make grade to ensure that your code passes all of the tests. The Gradescope autograder will use the same grading program to assign your submission a grade.
- Commit any modified source code before running make zipball .
- You can inspect the status of your submission and download the submitted code at Gradescope. The Gradescope lab grade is your final lab grade.

## Optional Challenges:

- In this lab, the networking stack uses interrupts to handle ingress packet processing, but not egress packet processing. A more sophisticated strategy would be to queue egress packets in software and only provide a limited number to the NIC at any one time. You can then rely on TX interrupts to refill the transmit ring. Using this technique, it becomes possible to prioritize different types of egress traffic. ( easy )
- The provided networking code only partially supports ARP. Implement a full ARP cache . ( moderate )
- The E1000 supports multiple RX and TX rings. Configure the E1000 to provide a ring pair for each core and modify your networking stack to support multiple rings. Doing so has the potential to increase the throughput that your networking stack can support as well as reduce lock contention. ( moderate ), but difficult to test/measure
- ICMP can provide notifications of failed networking flows. Detect these notifications and propagate them as errors to the user process.
- The E1000 supports several stateless hardware offloads, including checksum calculation, RSC, and GRO. Use one or more of these offloads to increase the throughput of your networking stack. ( moderate ), but hard to test/measure
- The networking stack in this lab is susceptible to receive livelock. Using the material in lecture and the reading assignment, devise and implement a solution to fix it. ( moderate ), but hard to test.
- Implement a minimal TCP stack and download a web page. ( hard )

Some of these challenges are intended to increase performance in ways that may not be apparent or measurable under QEMU.

If you pursue a challenge problem, whether it is related to networking or not, please let the course staff know!

Questions or comments regarding 6.1810? Send e-mail to the course staff at 61810-staff@lists.csail.mit.edu.