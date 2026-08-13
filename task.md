# Lab: mmap ( hard )

The `mmap` and `munmap` system calls allow UNIX programs to exert detailed control over their address spaces. They can be used to share memory among processes, to map files into process address spaces, and as part of user-level page fault schemes such as the garbage-collection algorithms discussed in lecture. In this lab you'll add `mmap` and `munmap` to xv6, focusing on memory-mapped files.

Fetch the xv6 source for the lab and check out the `mmap` branch:

```bash
$ git fetch
$ git checkout mmap
$ make clean
```

The manual page (run `man 2 mmap`) shows this declaration for `mmap`:

```bash
void *mmap(void *addr, size_t len, int prot, int flags,
           int fd, off_t offset);
```

`mmap` can be called in many ways, but this lab requires only a subset of its features relevant to memory-mapping a file. You can assume that `addr` will always be zero, meaning that the kernel should decide the virtual address at which to map the file.`mmap` returns that address, or 0xffffffffffffffff if it fails.`len` is the number of bytes to map; it might not be the same as the file's length.`prot` indicates whether the memory should be mapped readable, writeable, and/or executable; you can assume that `prot` is `PROT_READ` or `PROT_WRITE` or both.`flags` will be either `MAP_SHARED`, meaning that modifications to the mapped memory should be written back to the file, or `MAP_PRIVATE`, meaning that they should not. You don't have to implement any other bits in `flags`.`fd` is the open file descriptor of the file to map. You can assume `offset` is zero (it's the starting point in the file at which to map).

Your implementation should fill in the page table lazily, in response to page faults. That is, `mmap` itself should not allocate physical memory or read the file. Instead, do that in page fault handling code in (or called by) `usertrap`, as in the copy-on-write lab. The reason to be lazy is to ensure that `mmap` of a large file is fast, and that `mmap` of a file larger than physical memory is possible.

It's OK if processes that map the same `MAP_SHARED` file do not share physical pages.

The manual page (run man 2 munmap) shows this declaration for `munmap`:

```bash
int munmap(void *addr, size_t len);
```

`munmap` should remove mmap mappings in the indicated address range, if any. If the process has modified the memory and has it mapped `MAP_SHARED`, the modifications should first be written to the file. An `munmap` call might cover only a portion of an mmap-ed region, but you can assume that it will either unmap at the start, or at the end, or the whole region (but not punch a hole in the middle of a region). When a process exits, any modifictions it has made to `MAP_SHARED` regions should be written to the relevant files, as if the process had called `munmap`.

> You should implement enough `mmap` and `munmap` functionality to make the `mmaptest` test program work. If `mmaptest` doesn't use a `mmap` feature, you don't need to implement that feature. You must also ensure that `usertests -q` continues to work.

When you're done, you should see output similar to this:

```bash
$ mmaptest
test basic mmap
test basic mmap: OK
test mmap private
test mmap private: OK
test mmap read-only
test mmap read-only: OK
test mmap read/write
test mmap read/write: OK
test mmap dirty
test mmap dirty: OK
test not-mapped unmap
test not-mapped unmap: OK
test lazy access
test lazy access: OK
test mmap two files
test mmap two files: OK
test fork
test fork: OK
test munmap prevents access
usertrap(): unexpected scause 0xd pid=7
            sepc=0x924 stval=0xc0001000
usertrap(): unexpected scause 0xd pid=8
            sepc=0x9ac stval=0xc0000000
test munmap prevents access: OK
test writes to read-only mapped memory
usertrap(): unexpected scause 0xf pid=9
            sepc=0xaf4 stval=0xc0000000
test writes to read-only mapped memory: OK
mmaptest: all tests succeeded
$ usertests -q
usertests starting
...
ALL TESTS PASSED
$
```

Here are some hints:

- Start by adding `_mmaptest` to `UPROGS` , and `mmap` and `munmap` system calls, in order to get `user/mmaptest.c` to compile. For now, just return errors from `mmap` and `munmap`. We defined `PROT_READ` etc for you in `kernel/fcntl.h`. Run `mmaptest`, which will fail at the first mmap call.
- Keep track of what `mmap` has mapped for each process. Define a structure corresponding to the VMA (virtual memory area) described in the "virtual memory for applications" lecture. This should record the address, length, permissions, file, etc. for a virtual memory range created by `mmap`. Since the xv6 kernel doesn't have a variable-size memory allocator in the kernel, it's OK to declare a fixed-size array of VMAs and allocate from that array as needed. A size of 16 should be sufficient.
- Implement `mmap`: find an unused region in the process's address space in which to map the file, and add a VMA to the process's table of mapped regions. The VMA should contain a pointer to a `struct file` for the file being mapped;`mmap` should increase the file's reference count so that the structure doesn't disappear when the file is closed(hint: see `filedup`). Run `mmaptest`: the first `mmap` should succeed, but the first access to the mmap-ed memory will cause a page fault and kill `mmaptest`.
- Add code to cause a page-fault in a mmap-ed region to allocate a page of physical memory, read 4096 bytes of the relevant file into that page, and map it into the user address space. Read the file with `readi`, which takes an offset argument at which to read in the file (but you will have to lock/unlock the inode passed to `readi`). Don't forget to set the permissions correctly on the page. Run `mmaptest`; it should get to the first `munmap`.
- Implement `munmap`: find the VMA for the address range and unmap the specified pages (hint: use `uvmunmap`). If `munmap` removes all pages of a previous `mmap`, it should decrement the reference count of the corresponding `struct file`. If an unmapped page has been modified and the file is mapped `MAP_SHARED`, write the page back to the file. Look at `filewrite` for inspiration.
- Ideally your implementation would only write back `MAP_SHARED` pages that the program actually modified. The dirty bit (`D`) in the RISC-V PTE indicates whether a page has been written. However,`mmaptest` does not check that non-dirty pages are not written back; thus you can get away with writing pages back without looking at `D` bits.
- Modify `exit` to unmap the process's mapped regions as if `munmap` had been called. Run `mmaptest`; all tests through `test mmap two files` should pass, but probably not `test fork`.
- Modify `fork` to ensure that the child has the same mapped regions as the parent. Don't forget to increment the reference count for a VMA's `struct file`. In the page fault handler of the child, it is OK to allocate a new physical page instead of sharing a page with the parent. The latter would be cooler, but it would require more implementation work. Run `mmaptest`; it should pass all the tests.

Run `usertests -q` to make sure everything still works.

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

## Optional challenges

- If two processes have the same file mmap-ed (as in the fork tests), share their physical pages. You will need reference counts on physical pages.
- Your solution probably allocates a new physical page for each page read from the mmap-ed file, even though the data is also in kernel memory in the buffer cache. Modify your implementation to use that physical memory, instead of allocating a new page. This requires that file blocks be the same size as pages (set `BSIZE` to 4096). You will need to pin mmap-ed blocks into the buffer cache. You will need worry about reference counts. One benefit of fixing this double-caching is that it also helps make `read()` and `write()` consistent with `mmap`. That is, if some mmaped file data is modified through the memory mapping, `read` should return those modifications, and likewise, if an application calls `write`, the write should appear in any active memory mappings of that file. You might find it interesting to read the paper on the unified buffer cache .
- Remove redundancy between your implementation for lazy allocation and your implementation of mmap-ed files. (Hint: create a VMA for the lazy allocation area.)
- Modify `exec` to use a VMA for different sections of the binary so that you get on-demand-paged executables. This will make starting programs faster, because `exec` will not have to read any data from the file system.
- Implement page-out and page-in: have the kernel move some parts of processes to disk when physical memory is low. Then, page in the paged-out memory when the process references it.
