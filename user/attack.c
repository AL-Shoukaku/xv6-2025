#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // Your code here.
  char *data;
  data = sbrk(8*4096);
  for (int i = 0;i < 8*4096;i++) {
    if (strcmp(data,"This may help.") == 0) {
      data += 16;
      printf("%s\n", data);
      exit(0);
    }
    data++;
  }
  exit(1);
}
