/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 517
 * Language: c
 * Block ID: 25d3b25a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/mmaptest.c
#include "kernel/types.h"
#include "user/user.h"

void test_basic_mmap() {
    int fd;
    char *addr;

    // 创建测试文件
    fd = open("testfile", O_CREATE | O_RDWR);
    write(fd, "hello mmap!\n", 12);
    close(fd);

    // 映射文件
    fd = open("testfile", O_RDONLY);
    addr = mmap(0, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (addr == (char *)-1) {
        printf("mmap failed\n");
        exit(1);
    }

    // 读取映射内存
    printf("mapped content: %s", addr);

    // 解除映射
    munmap(addr, 4096);

    // 清理
    unlink("testfile");

    printf("basic mmap test passed\n");
}

void test_private_write() {
    int fd;
    char *addr;

    // 创建测试文件
    fd = open("testfile", O_CREATE | O_RDWR);
    write(fd, "original\n", 9);
    close(fd);

    // 映射文件
    fd = open("testfile", O_RDWR);
    addr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    // 修改映射内存 (触发COW)
    strcpy(addr, "modified\n");

    // 读取验证
    if (strcmp(addr, "modified\n") != 0) {
        printf("write to private mapping failed\n");
        exit(1);
    }

    munmap(addr, 4096);

    // 验证原文件未被修改
    char buf[20];
    fd = open("testfile", O_RDONLY);
    read(fd, buf, sizeof(buf));
    close(fd);

    if (strcmp(buf, "original\n") != 0) {
        printf("private mapping modified file!\n");
        exit(1);
    }

    unlink("testfile");
    printf("private write test passed\n");
}

void test_anonymous() {
    char *addr;

    // 匿名映射
    addr = mmap(0, 4096, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (addr == (char *)-1) {
        printf("anonymous mmap failed\n");
        exit(1);
    }

    // 应该初始化为0
    for (int i = 0; i < 4096; i++) {
        if (addr[i] != 0) {
            printf("anonymous mapping not zeroed\n");
            exit(1);
        }
    }

    // 写入测试
    strcpy(addr, "test");

    munmap(addr, 4096);
    printf("anonymous mmap test passed\n");
}

int main() {
    test_basic_mmap();
    test_private_write();
    test_anonymous();
    printf("All mmap tests passed!\n");
    exit(0);
}
