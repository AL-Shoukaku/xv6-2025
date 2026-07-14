# Lab: Xv6 and Unix utilities

This lab will familiarize you with xv6 and its system calls.

## Boot xv6 ( easy )

Have a look at the lab tools page for information about how to set up your computer to run these labs.

Fetch the git repository for the xv6 source for the lab:

```bash
$ git clone git://g.csail.mit.edu/xv6-labs-2025
Cloning into 'xv6-labs-2025'...
...
$ cd xv6-labs-2025
```

The files you will need for this and subsequent labs are distributed using the Git version control system. For each of the labs you will check out a version of xv6 tailored for that lab. To learn more about Git, take a look at the Git user's manual, or this CS-oriented overview of Git. Git allows you to keep track of the changes you make to the code. For example, if you are finished with one of the exercises, and want to checkpoint your progress, you can commit your changes by running:

```bash
$ git commit -am 'my solution for util lab exercise 1'
Created commit 60d2135: my solution for util lab exercise 1
 1 files changed, 1 insertions(+), 0 deletions(-)
$
```

You can view your changes with git diff, which displays changes since your last commit. git diff origin/util displays changes relative to the initial util code.origin/util is the name of the git branch for this lab.

Build and run xv6:

```bash
$ make qemu
riscv64-unknown-elf-gcc    -c -o kernel/entry.o kernel/entry.S
riscv64-unknown-elf-gcc -Wall -Werror -O -fno-omit-frame-pointer -ggdb -DSOL_UTIL -MD -mcmodel=medany -ffreestanding -fno-common -nostdlib -mno-relax -I. -fno-stack-protector -fno-pie -no-pie   -c -o kernel/start.o kernel/start.c
...
riscv64-unknown-elf-ld -z max-page-size=4096 -N -e main -Ttext 0 -o user/_zombie user/zombie.o user/ulib.o user/usys.o user/printf.o user/umalloc.o
riscv64-unknown-elf-objdump -S user/_zombie > user/zombie.asm
riscv64-unknown-elf-objdump -t user/_zombie | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$/d' > user/zombie.sym
mkfs/mkfs fs.img README  user/xargstest.sh user/_cat user/_echo user/_forktest user/_grep user/_init user/_kill user/_ln user/_ls user/_mkdir user/_rm user/_sh user/_stressfs user/_usertests user/_grind user/_wc user/_zombie
nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 954 total 1000
balloc: first 591 blocks have been allocated
balloc: write bitmap block at sector 45
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$
```

If you type `ls` at the prompt, you should see output similar to the following:

```bash
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2227
xargstest.sh   2 3 93
cat            2 4 32864
echo           2 5 31720
forktest       2 6 15856
grep           2 7 36240
init           2 8 32216
kill           2 9 31680
ln             2 10 31504
ls             2 11 34808
mkdir          2 12 31736
rm             2 13 31720
sh             2 14 54168
stressfs       2 15 32608
usertests      2 16 178800
grind          2 17 47528
wc             2 18 33816
zombie         2 19 31080
console        3 20 0
```

These are the files that `mkfs` includes in the initial file system; most are programs you can run. You just ran one of them: `ls`.

xv6 has no `ps` command, but, if you type Ctrl-p, the kernel will print information about each process. If you try it now, you'll see two lines: one for init, and one for `sh`.

To quit qemu type: Ctrl-a x (press Ctrl and a at the same time, followed by x).

## sleep ( easy )

This exercise makes you familiar with writing a user program on xv6 and the pause system call.

>Implement a user-level sleep program for xv6, along the lines of the UNIX sleep command. Your sleep should pause for a user-specified number of ticks. A tick is a notion of time defined by the xv6 kernel, namely the time between two interrupts from the timer chip. Your solution should be in the file user/sleep.c.

Some hints:
- Before you start coding, read Chapter 1 of the xv6 book .
- Put your code in `user/sleep.c`. Look at some of the other programs in user/(e.g.,`user/echo.c`,`user/grep.c`, and `user/rm.c`) to see how command-line arguments are passed to a program.
- Add your `sleep` program to `UPROGS`in Makefile; once you've done that,`make qemu` will compile your program and you'll be able to run it from the xv6 shell.
- If the user forgets to pass an argument, sleep should print an error message.
- The command-line argument is passed as a string; you can convert it to an integer using `atoi`(see `user/ulib.c`).
- Use the system call`pause()`.
- See `kernel/sysproc.c` for the xv6 kernel code that implements the `pause()` system call (look for `sys_pause`),`user/user.h` for the C definition of `pause()` callable from a user program, and `user/usys.S` for the assembler code that jumps from user code into the kernel for `pause()`.
- Look at Kernighan and Ritchie's book The C programming language (second edition) (K&R) to learn about C.

Run the program from the xv6 shell:

```bash
$ make qemu
...
init: starting sh
$ sleep 10
(nothing happens for a little while)
$
```

Your program should pause when run as shown above. Run make grade in your command line (outside of qemu) to see if you pass the sleep tests.

Note that make grade runs all tests, including the ones for the tasks below. If you want to run the grade tests for one task, type:

```bash
$ ./grade-lab-util sleep
```

This will run the grade tests that match "sleep". Or, you can type:

```bash
$ make GRADEFLAGS=sleep grade
```

which does the same.

## sixfive ( moderate )

In this exercise you'll use the system calls open and read, C strings, and processing text files in C.

> For each input file, `sixfive`must print all the numbers in the file that are multiples of 5 or 6. Number are a sequence of decimal digits separated by characters in the string " -\r\t\n./,". Thus, for the six in "xv6" sixfive shouldn't print 6 but, for example, "/6," it should.

The following example illustrates sixfive's behavior:

```bash
$ sixfive sixfive.txt
5
100
18
6
$
```

Some hints:

- Read the input file a character at the time
- You can test if a character matches any of the separators using strchr(see `user/ulib.c`).
- Start and end of file are implicit separators.

## memdump ( easy )

This exercise will give you more practice using C pointers. Before starting read section 5.1 (Pointers and addresses) through 5.6 (Pointer arrays) and 6.4 (pointers to structures) in "The C programming language (second edition)" by Kernighan and Ritchie (K&R).

Have a look at `user/memdump.c`. Your job is to implement the function `memdump(char *fmt, char *data)`.`memdump()`'s purpose is to print the contents of the memory pointed to by data in the format described by the fmt argument. The format is a C string. Each character of the string indicates how to print successive parts of the data. Thus, for example, a C struct with multiple fields can be printed with a format string containing multiple characters.

Your memdump() should handle the following format characters:

- i: print the next 4 bytes of the data as a 32-bit integer, in decimal.
- p: print the next 8 bytes of the data as a 64-bit integer, in hex.
- h: print the next 2 bytes of the data as a 16-bit integer, in decimal.
- c: print the next 1 byte of the data as an 8-bit ASCII character.
- s: the next 8 bytes of the data contain a 64-bit pointer to a C string; print the string.
- S: the rest of the data contains the bytes of a null-terminated C string; print the string.

Feel free to use C's `printf()` in your `memdump()`.

The `memdump` program, if executed with no arguments, calls `memdump()` with some example format strings and data. If `memdump()` is correctly implemented, the output will be:

```bash
$ memdump
Example 1:
61810
2025
Example 2:
a string
Example 3:
another
Example 4:
BD0
1819438967
100
z
xyzzy
Example 5:
hello
w
o
r
l
d
```

You will likely get a different hex address for the first line of the Example 4 output.

If the `memdump` program is invoked with an argument, it will read its standard input up to an end of file, and then call `memdump()` with the format and input data. So, once `memdump()` is implemented:

```bash
$ echo deadc0de | memdump hhcccc
25956
25697
c
0
d
e
$ echo deadc0de | memdump p
64616564
$
```

> Implement `memdump()`.

## find ( moderate )

This exercise explores pathnames and directories, and the system calls open, read, and fstat.

> Write a simple version of the UNIX find program for xv6: find all the files in a directory tree with a specific name. Your solution should be in the file `user/find.c`.

Some hints:

- Look at user/ls.c to see how to read a directory.
- Use recursion to allow find to descend into sub-directories.
- Don't recurse into "." and "..".
- Each time you invoke make qemu , it will build a new fs.img, removing files created in a previous run. If you would like to start qemu with the file system from a previous use make qemu-fs .
- You'll need to use C strings. Have a look at K&R (the C book), for example Section 5.5.
- Note that == does not compare strings as in Python. Use strcmp() instead.
- Add the program to `UPROGS` in Makefile.

Your solution should produce the following output (when the file system contains the files `b`,`a/b` and `a/aa/b`):

```bash
$ make qemu
...
init: starting sh
$ echo > b
$ mkdir a
$ echo > a/b
$ mkdir a/aa
$ echo > a/aa/b
$ find . b
./b
./a/b
./a/aa/b
$
```

Run `make grade` to see what our tests think.

## exec ( moderate )

This exercise involves the system calls fork, exec, and wait.

> Add a "-exec cmd" to `find`, which executes the program "`cmd file`" for each file f that `find` finds, instead of printing matching file names.

The following example illustrates find -exec behavior:

```
$ find . wc -exec echo hi
hi ./wc
$
```

Note that the command here is "echo hi" and the file is "./wc", making the command "echo hi ./wc", which outputs "hi ./wc".

Some hints:

- Use `fork` and `exec` to invoke the command on each file. Use `wait` in the parent to wait for the child to complete the command.
- `kernel/param.h` declares MAXARG, which may be useful if you need to declare an argv array.

To test your solution for find, run the shell script findtest.sh. Your solution should produce the following output:

```bash
  $ make qemu
  ...
  init: starting sh
  $ sh < findtest.sh
$ echo DONE
$ $ $ $ $ hello
hello
hello
$ $
```

The output has many \$ because the xv6 shell doesn't realize it is processing commands from a file instead of from the console, and prints a \$ for each command in the file.

## Submit the lab

### Time spent

Create a new file, `time.txt`, and put in a single integer, the number of hours you spent on the lab. git add and git commit the file.

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

Inspect the above lines and make sure all files that your lab solution needs are tracked, i.e., not listed in a line that begins with `??`. You can cause git to track a new file that you create using git add {filename}.

- Please run make grade to ensure that your code passes all of the tests. The Gradescope autograder will use the same grading program to assign your submission a grade.
- Commit any modified source code before running make zipball .
- You can inspect the status of your submission and download the submitted code at Gradescope. The Gradescope lab grade is your final lab grade.

## Optional challenge exercises

- Write an uptime program that prints the uptime in terms of ticks using the `uptime` system call. ( easy )
- Support regular expressions in name matching for `find`.`grep.c` has some primitive support for regular expressions. ( easy )
- The xv6 shell (`user/sh.c`) is just another user program. It lacks many features found in real shells, but you can modify and improve it. For example, modify the shell to not print a \$ when processing shell commands from a file ( moderate ), modify the shell to support wait ( easy ), modify the shell to support tab completion ( easy ), modify the shell to keep a history of passed shell commands ( moderate ), or anything else you would like your shell to do. (If you are very ambitious, you may have to modify the kernel to support the kernel features you need; xv6 doesn't support much.)

Questions or comments regarding 6.1810? Send e-mail to the course staff at 61810-staff@lists.csail.mit.edu.