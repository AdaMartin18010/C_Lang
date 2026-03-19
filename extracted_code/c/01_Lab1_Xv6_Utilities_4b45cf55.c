/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\01_Lab1_Xv6_Utilities.md
 * Line: 300
 * Language: c
 * Block ID: 4b45cf55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/find.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char *name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 打开目录
    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 必须是目录
    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    // 路径长度检查
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        printf("find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // 读取目录项
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        // 跳过 . 和 ..
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        // 构建完整路径
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // 获取文件状态
        if (stat(buf, &st) < 0) {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        // 如果是文件且匹配，打印
        if (st.type == T_FILE && strcmp(de.name, name) == 0) {
            printf("%s\n", buf);
        }

        // 如果是目录，递归查找
        if (st.type == T_DIR) {
            find(buf, name);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
