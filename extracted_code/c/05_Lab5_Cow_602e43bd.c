/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 482
 * Language: c
 * Block ID: 602e43bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/cowtest.c
#include "kernel/types.h"
#include "user/user.h"

void simple_test() {
    int pid;
    char *page = sbrk(4096);

    // 写入数据
    strcpy(page, "parent data");

    pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // 子进程
        printf("child before write: %s\n", page);

        // 写入触发COW
        strcpy(page, "child data");
        printf("child after write: %s\n", page);

        exit(0);
    } else {
        // 父进程
        wait(0);
        printf("parent after child: %s\n", page);

        if (strcmp(page, "parent data") != 0) {
            printf("ERROR: parent data corrupted!\n");
            exit(1);
        }
    }
}

void stress_test() {
    // 大量fork和写入测试
    for (int i = 0; i < 100; i++) {
        int pid = fork();
        if (pid == 0) {
            // 分配并写入内存
            char *mem = sbrk(4096 * 10);
            for (int j = 0; j < 10; j++) {
                mem[j * 4096] = 'a' + (j % 26);
            }
            exit(0);
        }
        wait(0);
    }
    printf("stress test passed\n");
}

int main() {
    printf("=== COW Test ===\n");
    simple_test();
    stress_test();
    printf("All tests passed!\n");
    exit(0);
}
