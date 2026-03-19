/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\01_Lab1_Xv6_Utilities.md
 * Line: 205
 * Language: c
 * Block ID: 2c279d13
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/primes.c
#include "kernel/types.h"
#include "user/user.h"

void sieve(int p[2]) {
    int prime;

    // 从上一个进程读取第一个数（就是素数）
    if (read(p[0], &prime, sizeof(prime)) == 0) {
        close(p[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    // 创建新管道给下一个进程
    int p2[2];
    pipe(p2);

    if (fork() == 0) {
        // 子进程：继续筛
        close(p[0]);
        close(p2[1]);
        sieve(p2);
    } else {
        // 当前进程：过滤倍数
        close(p2[0]);

        int n;
        while (read(p[0], &n, sizeof(n)) != 0) {
            if (n % prime != 0) {
                // 不是倍数，传给下一个进程
                write(p2[1], &n, sizeof(n));
            }
        }

        close(p[0]);
        close(p2[1]);
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // 子进程：开始筛
        close(p[1]);
        sieve(p);
    } else {
        // 父进程：生成2-35
        close(p[0]);

        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));
        }

        close(p[1]);
        wait(0);
        exit(0);
    }
}
