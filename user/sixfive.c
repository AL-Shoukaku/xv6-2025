#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"

int isSeparators(char c) {
    char separators[] = " -\r\t\n./,";
    for (int i = 0;i < strlen(separators);i++) {
        if (c == separators[i]) {
            return 1;
        }
    }
    return 0;
}

int isNum(char c) {
    if (c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

int str2int(char *s) {
    int num = 0;
    for (int i = 0;i < strlen(s);i++) {
        num = num * 10;
        num += s[i] - '0';
    }
    return num;
}

int
main(int argc, char *argv[]) 
{
    if (argc <= 1) {
        write(1, "please enter filename!\n", 23);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        char *c = (char *)malloc(sizeof(char *));
        *c = '\0';
        char *str = (char *)malloc(sizeof(char *));
        *str = '\0';
        int hasNum = 0; //分隔符间是否有数字
        while (read(fd, c, 1) == 1) {
            if (isSeparators(*c)) {
                //两个分隔符间有数字，且是5或6的倍数
                if (hasNum && (str2int(str) % 5 == 0 || str2int(str) % 6 == 0)) {
                    while (*str == '0' && *(str + 1) != '\0') {
                        str++;
                    }
                    write(1, str, strlen(str));
                    write(1, "\n", 1);
                }
                hasNum = 0;
                *str = '\0';
            } else if (isNum(*c)) {
                hasNum = 1;
                str[strlen(str) + 1] = '\0';
                str[strlen(str)] = *c;
            } else {
                //不是数字也不是分隔符，则不满足要求，不输出
                hasNum = 0;
                *str = '\0';
            }
        } 
        // 文件的结尾也是分隔符！
        if (hasNum && (str2int(str) % 5 == 0 || str2int(str) % 6 == 0)) {
            while (*str == '0' && *(str + 1) != '\0') {
                str++;
           }
 
            write(1, str, strlen(str));
            write(1, "\n", 1);
        }
    }
    exit(0);
}

