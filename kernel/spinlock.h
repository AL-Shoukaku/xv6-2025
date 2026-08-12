// Mutual exclusion lock.
#define LAB_LOCK 1
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
#ifdef LAB_LOCK
  int nts;
  int n;
#endif
};

#ifdef LAB_LOCK
// Reader-writer lock.
struct rwspinlock {
  // Replace this with your implementation.
  struct spinlock rlock;
  struct spinlock wlock;
  struct spinlock lock;
  uint64 reader;
};
#endif
