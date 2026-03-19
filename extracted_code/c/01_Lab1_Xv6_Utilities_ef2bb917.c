/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\01_Lab1_Xv6_Utilities.md
 * Line: 94
 * Language: c
 * Block ID: ef2bb917
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/pingpong.c
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];
    char buf[1];

    // 创建两个管道
    // p1: 父进程 -> 子进程
    // p2: 子进程 -> 父进程
    pipe(p1);
    pipe(p2);

    if (fork() == 0) {
        // 子进程
        close(p1[1]);  // 关闭p1写端
        close(p2[0]);  // 关闭p2读端

        // 从父进程接收
        if (read(p1[0], buf, 1) != 1) {
            fprintf(2, "child read failed\n");
            exit(1);
        }

        // 打印接收信息
        printf("%d: received ping\n", getpid());

        // 回复父进程
        if (write(p2[1], buf, 1) != 1) {
            fprintf(2, "child write failed\n");
            exit(1);
        }

        close(p1[0]);
        close(p2[1]);
        exit(0);

    } else {
        // 父进程
        close(p1[0]);  // 关闭p1读端
        close(p2[1]);  // 关闭p2写端

        // 发送给子进程
        buf[0] = 'x';
        if (write(p1[1], buf, 1) != 1) {
            fprintf(2, "parent write failed\n");
            exit(1);
        }

        // 等待子进程回复
        if (read(p2[0], buf, 1) != 1) {
            fprintf(2, "parent read failed\n");
            exit(1);
        }

        printf("%d: received pong\n", getpid());

        close(p1[1]);
        close(p2[0]);

        // 等待子进程结束
        wait(0);
        exit(0);
    }
}
