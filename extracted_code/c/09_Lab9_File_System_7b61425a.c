/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 655
 * Language: c
 * Block ID: 7b61425a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/fstest.c
#include "kernel/types.h"
#include "user/user.h"

void test_create_write_read() {
    int fd;
    char buf[100];

    // 创建并写入
    fd = open("testfile", O_CREATE | O_WRONLY);
    write(fd, "hello world\n", 12);
    close(fd);

    // 读取
    fd = open("testfile", O_RDONLY);
    int n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("read: %s", buf);
    close(fd);

    // 删除
    unlink("testfile");
}

void test_symlink() {
    int fd;
    char buf[100];

    // 创建目标文件
    fd = open("target", O_CREATE | O_WRONLY);
    write(fd, "target content\n", 15);
    close(fd);

    // 创建符号链接
    symlink("target", "link");

    // 通过链接读取
    fd = open("link", O_RDONLY);
    read(fd, buf, sizeof(buf));
    printf("via symlink: %s", buf);
    close(fd);

    // 删除目标文件
    unlink("target");

    // 尝试读取悬挂链接
    fd = open("link", O_RDONLY);
    if (fd < 0) {
        printf("dangling link correctly detected\n");
    }

    unlink("link");
}

int main() {
    test_create_write_read();
    test_symlink();
    exit(0);
}
