/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\00_MIT_Labs_Overview.md
 * Line: 79
 * Language: c
 * Block ID: f41ccc8c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/pingpong.c
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];
    pipe(p1); pipe(p2);

    if (fork() == 0) {
        // 子进程
        char buf[1];
        read(p1[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(p2[1], buf, 1);
        exit(0);
    } else {
        // 父进程
        char buf[1] = {'x'};
        write(p1[1], buf, 1);
        read(p2[0], buf, 1);
        printf("%d: received pong\n", getpid());
        wait(0);
        exit(0);
    }
}
