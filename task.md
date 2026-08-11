# Lab: locks

In this lab you'll gain experience in re-designing code to increase parallelism. A common symptom of poor parallelism on multi-core machines is high lock contention. Improving parallelism often involves changing both data structures and locking strategies in order to reduce contention. You'll do this for the xv6 memory allocator and block cache.

> Before writing code, make sure to read the following parts from the xv6 book :
> 
> - Chapter 7: "Locking" and the corresponding code.
> - Section 3.5: "Code: Physical memory allocator"

```bash
$ git fetch
$ git checkout lock
$ make clean
```

## Memory allocator ( moderate )

The program user/kalloctest stresses xv6's memory allocator: three processes grow and shrink their address spaces, resulting in many calls to `kalloc` and `kfree`. `kalloc` and `kfree` obtain `kmem.lock`. kalloctest prints (as "\#test-and-set") the number of loop iterations in `acquire` due to attempts to acquire a lock that another core already holds, for the `kmem` lock and a few other locks. The number of loop iterations in `acquire` is a rough measure of lock contention. The output of `kalloctest` looks similar to this before you start the lab:

```bash
$ kalloctest
start test1
test1 results:
--- lock kmem/bcache stats
lock: kmem: #test-and-set 18820 #acquire() 433058
lock: bcache: #test-and-set 0 #acquire() 1744
--- top 5 contended locks:
lock: uart: #test-and-set 97375 #acquire() 117
lock: virtio_disk: #test-and-set 96764 #acquire() 183
lock: proc: #test-and-set 45428 #acquire() 522884
lock: proc: #test-and-set 25506 #acquire() 522875
lock: kmem: #test-and-set 18820 #acquire() 433058
tot= 18820
test1 FAIL
start test2
total free number of pages: 32463 (out of 32768)
..........
test2 OK
start test3
..........child done 10000

test3 OK
start test4
..........child done 100000
--- lock kmem/bcache stats
lock: kmem: #test-and-set 135309 #acquire() 3006254
lock: bcache: #test-and-set 0 #acquire() 1860
--- top 5 contended locks:
lock: wait_lock: #test-and-set 37427486 #acquire() 40008
lock: proc: #test-and-set 3108381 #acquire() 1689532
lock: proc: #test-and-set 207812 #acquire() 1588761
lock: kmem: #test-and-set 135309 #acquire() 3006254
lock: uart: #test-and-set 97375 #acquire() 1303
tot= 135309
test4 FAIL m 59382 n 135309
$
```

You'll likely see different counts than shown here, and a different order for the top 5 contended locks.

`acquire` maintains, for each lock, the count of calls to `acquire` for that lock, and the number of times the loop in `acquire` tried but failed to set the lock. kalloctest calls a system call that causes the kernel to print those counts for the kmem locks (which are the focus of this lab) and for the 5 most contended locks. If there is lock contention, the number of `acquire` loop iterations will be large. The system call returns the sum of the number of loop iterations for the kmem locks.

For this lab, you must use a dedicated unloaded machine with multiple cores. If you use a machine that is doing other things, the counts that kalloctest prints will be nonsense. You can use a dedicated Athena workstation, or your own laptop, but don't use a dialup machine.

The root cause of lock contention in kalloctest is that `kalloc()` has a single free list, protected by a single lock. To remove lock contention, you will have to redesign the memory allocator to avoid a single lock and list. The basic idea is to maintain a free list per CPU, each list with its own lock. Allocations and frees on different CPUs can run in parallel, because each CPU will operate on a different list. The main challenge will be to deal with the case in which one CPU's free list is empty, but another CPU's list has free memory; in that case, the one CPU must "steal" part of the other CPU's free list. Stealing may introduce lock contention, but that will hopefully be infrequent.

> Your job is to implement per-CPU freelists, and stealing when a CPU's free list is empty. You must give all of your locks names that start with "kmem". That is, you should call `initlock` for each of your locks, and pass a name that starts with "kmem". Run kalloctest to see if your implementation has reduced lock contention. To check that it can still allocate all of memory, run `usertests sbrkmuch`. Your output will look similar to that shown below, with much-reduced contention in total on kmem locks, although the specific numbers will differ. Make sure all tests in `usertests -q` pass. `make grade` should say that the kalloctests pass.

```bash
$ kalloctest
start test1
test1 results:
--- lock kmem/bcache stats
lock: kmem: #test-and-set 0 #acquire() 117911
lock: kmem: #test-and-set 0 #acquire() 169464
lock: kmem: #test-and-set 0 #acquire() 145786
lock: bcache: #test-and-set 0 #acquire() 1744
--- top 5 contended locks:
lock: virtio_disk: #test-and-set 93747 #acquire() 183
lock: proc: #test-and-set 44010 #acquire() 526316
lock: proc: #test-and-set 24347 #acquire() 526309
lock: wait_lock: #test-and-set 11726 #acquire() 12
lock: pr: #test-and-set 4579 #acquire() 5
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
..........child done 100000
--- lock kmem/bcache stats
lock: kmem: #test-and-set 3673 #acquire() 827384
lock: kmem: #test-and-set 3449 #acquire() 1215152
lock: kmem: #test-and-set 1924 #acquire() 1236349
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: kmem: #test-and-set 0 #acquire() 1014
lock: bcache: #test-and-set 0 #acquire() 1860
--- top 5 contended locks:
lock: wait_lock: #test-and-set 39121537 #acquire() 40020
lock: proc: #test-and-set 6853704 #acquire() 1672258
lock: proc: #test-and-set 214194 #acquire() 1614201
lock: uart: #test-and-set 195773 #acquire() 1459
lock: virtio_disk: #test-and-set 93747 #acquire() 183
tot= 9046

test4 OK
$ usertests sbrkmuch
usertests starting
test sbrkmuch: OK
ALL TESTS PASSED
$ usertests -q
...
ALL TESTS PASSED
$
```

Some hints:

- You can use the constant `NCPU` from `kernel/param.h`
- Let `freerange`
give all free memory to the CPU running `freerange`.
- The function `cpuid`
returns the current core number, but it's only safe to call it and use its result when interrupts are turned off. You should use `push_off()` and `pop_off()` to turn interrupts off and on.
- Have a look at the `snprintf` function in kernel/sprintf.c for string formatting ideas. It is OK to just name all locks "kmem" though.
- Optionally run your solution using xv6's race detector:
```bash
$ make clean
$ make KCSAN=1 qemu
$ kalloctest
  ..
```
- The `kalloctest` may fail but you shouldn't see any races. If the xv6's race detector observes a race, it will print two stack traces describing the races along the following lines:

```bash
== race detected ==
backtrace for racing load
0x000000008000ab8a
0x000000008000ac8a
0x000000008000ae7e
0x0000000080000216
0x00000000800002e0
0x0000000080000f54
0x0000000080001d56
0x0000000080003704
0x0000000080003522
0x0000000080002fdc
backtrace for watchpoint:
0x000000008000ad28
0x000000008000af22
0x000000008000023c
0x0000000080000292
0x0000000080000316
0x000000008000098c
0x0000000080000ad2
0x000000008000113a
0x0000000080001df2
0x000000008000364c
0x0000000080003522
0x0000000080002fdc
==========
```

On your OS, you can turn a backtrace into function names with line numbers by cutting and pasting it into
`addr2line`:

```bash
	 $ riscv64-linux-gnu-addr2line -e kernel/kernel
	 0x000000008000ab8a
	 0x000000008000ac8a
	 0x000000008000ae7e
	 0x0000000080000216
	 0x00000000800002e0
	 0x0000000080000f54
	 0x0000000080001d56
	 0x0000000080003704
	 0x0000000080003522
	 0x0000000080002fdc
	ctrl-d
	kernel/kcsan.c:157
        kernel/kcsan.c:241
        kernel/kalloc.c:174
        kernel/kalloc.c:211
        kernel/vm.c:255
        kernel/proc.c:295
        kernel/sysproc.c:54
        kernel/syscall.c:251
```

- You are not required to run the race detector, but you might find it helpful. Note that the race detector slows xv6 down significantly, so you probably don't want to use it when running `usertests`.

## Read-write lock ( moderate )

This half of the assignment is independent from the first half; you can work on this half (and pass the tests) whether or not you have completed the first half.

Consider xv6's `sys_pause` and `sys_uptime` functions, which read the global `ticks` variable. Since that variable might be concurrently updated by `clockintr`, these two functions acquire the `tickslock` spinlock before reading `ticks`. Importantly, this prevents `clockintr` from concurrently modifying `ticks`, but this also prevents multiple cores from reading `ticks` at the same time, which would have been just fine. In the latter case, the spinlock needlessly reduces performance.

A common solution to this problem is to use a read-write lock. Read-write locks introduce the notion of two kinds of lock holders: readers and writers. There can be at most one writer at a time (and when there's a writer, there can be no readers), but there can be many readers at the same time (as long as there's no writer). The typical read-write lock API is as follows (see `kernel/defs.h`):

```bash
void            initrwlock(struct rwspinlock*);
void            read_acquire(struct rwspinlock*);
void            read_release(struct rwspinlock*);
void            write_acquire(struct rwspinlock*);
void            write_release(struct rwspinlock*);
```

One subtle issue that can arise with read-write locks is that, if there are many readers, then writers might never get a chance to run. In other words, even though readers keep acquiring and releasing the lock, there's never a point when there are actually zero readers, so a writer might never be able to acquire the lock. To address this problem, read-write locks typically implement a writer priority scheme: once a writer tries to acquire the lock, subsequent readers must wait until the writer succeeds in acquiring and releasing the lock (of course, waiting for the current readers to release the lock before allowing the writer to acquire).

> Implement a read-write spinlock in xv6, following the API and semantics described above. Make sure that readers do not starve writers: if there's any pending writers, no more readers can acquire the lock.
>
> You will need to fill in the stubbed-out functions for the read-write spinlock API in `kernel/spinlock.c` , and likely change the definition of a `struct rwspinlock` in `kernel/spinlock.h`.
>
> When you are done, test your rwspinlock implementation by running `rwlktest`. You should see output similar to the following:
>
> ```bash
> $ rwlktest
> rwspinlock_test: step 1
> rwspinlock_test: step 2
> rwspinlock_test: step 3
> rwspinlock_test: step 4
> rwspinlock_test: step 5
> rwspinlock_test: step 6
> rwspinlock_test: step 7
> rwspinlock_test(0): 0
> rwspinlock_test(2): 0
> rwspinlock_test(1): 0
> rwlktest: passed 3/3
> $
> ```
>
> Make sure `usertests -q` still passes.`make grade` should pass all tests when you are done.

Some hints:

- Take a look at `sys_rwlktest` in `kernel/spinlock.c` first; you should be able to test your implementation in stages.
- If you run `rwlktest` inside xv6 without modifying anything in `kernel/spinlock.c`, the kernel will print `panic: acquire` because the original spinlock implementation only allows the lock to be held by one thread at a time. You'll need to replace the calls to `acquire()` and `release()` in `write/read_acquire_inner` and `write/read_release_inner` with your own lock implementation.
- Be careful about possible interleavings. For example, if a reader sees no pending writer and wants to acquire the lock for reading, how do you know its still safe to acquire the lock?
- It might be helpful to read about the built-in gcc functions for atomic operations; see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html .

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