#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

void find(char *path, char *name);
void findexec(char *path, char *name, char **argv, int *argNum);
char* fmtname(char *path);

int main(int argc, char *argv[])
{
    if (argc < 3) {
        write(2, "argument error!\n", 15);
        exit(1);
    }
    int pos = 0;
    for (int i = 3;i < argc;i++) {
        if (strcmp("-exec", argv[i]) == 0) {
            pos = i;
            break;
        }
    }

    //存在-exec参数
    if (pos > 0) {
        char* newArgv[MAXARG];
        int argNum = 0;
        //先构建指令参数
        for (int i = pos + 1; i < argc; i++) {
            newArgv[argNum++] = argv[i];
        }
        //构建find指令找到的文件名作为参数
        for (int i = 2; i < pos; i++) {
            findexec(argv[1], argv[i], newArgv, &argNum);
        }

        int pid = fork();
        if (pid > 0) {
            wait((int *) 0);
        } else if (pid == 0){
           exec(argv[pos + 1], newArgv);
           printf("findexec: exec error!\n");
           exit(1);
        } else {
            printf("fork error!\n");
            exit(1);
        }
        exit(0);
    }

    //不存在-exec参数，正常执行
    char* path = argv[1];
    for (int i = 2;i < argc;i++) {
        char* filename = argv[i];
        find(path, filename);
    }
    exit(0);
}

void find(char *path, char *name) {
    int fd;
    struct stat st;
    char buf[512], *p;
    struct dirent de;
    if (strcmp(fmtname(path), name) == 0) {
        printf("%s\n", path);
    }
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 对于非目录的直接关闭fd，否则会数量不够 
    if (st.type != T_DIR) {
        close(fd);
        return;
    }
            
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("find: path too long\n");
        return;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[strlen(de.name)] = '\0';
        find(buf, name);
    }
    close(fd);
    return;
}

void findexec(char *path, char *name, char **argv, int *argNum) {
    int fd;
    struct stat st;
    char buf[512], *p;
    struct dirent de;
    if (strcmp(fmtname(path), name) == 0) {
        if (*argNum + 1 == MAXARG) {
            printf("findexec: too much argument!\n");
            return;
        }
        //这里不能直接用path！因为*path的值后面会被改变！
        char s[512];
        strcpy(s, path);
        argv[*argNum] = s;
        *argNum += 1;
    }

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "findexec: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "findexec: cannot stat %s\n", path);
        close(fd);
        return;
    }
    // 对于非目录的直接关闭fd，否则会数量不够 
    if (st.type != T_DIR) {
        close(fd);
        return;
    }
            
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("findexec: path too long\n");
        return;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[strlen(de.name)] = '\0';
        findexec(buf, name, argv, argNum);
    }
    close(fd);
 
}

char* fmtname(char *path)
{
  //static char buf[DIRSIZ+1]; 这里用不到
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

//ls原版里会凑长度，这里不能，否则影响strcmp的判断
    return p;
  // Return blank-padded name.
  //if(strlen(p) >= DIRSIZ)
  //  return p;
  //memmove(buf, p, strlen(p));
  //memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  //buf[sizeof(buf)-1] = '\0';
  //return buf;
}

