#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc <= 1) {
        write(1, "no arguments!\n", 14);
        exit(1);
    }
 
    int time = atoi(argv[1]);
    if (pause(time) < 0) {
        write(1, "sleep fail\n", 11);
        exit(1);
    }
 
    exit(0);
}
