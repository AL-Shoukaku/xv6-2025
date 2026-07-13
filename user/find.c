#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *name);
char* fmtname(char *path);

int main(int argc, char *argv[])
{
    if (argc < 3) {
        write(2, "argument error!\n", 15);
        exit(1);
    }
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

