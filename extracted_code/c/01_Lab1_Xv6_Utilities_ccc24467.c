/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\01_Lab1_Xv6_Utilities.md
 * Line: 397
 * Language: c
 * Block ID: ccc24467
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/xargs.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    char buf[512];
    char *args[MAXARG];
    int i;

    // 复制原始参数
    for (i = 1; i < argc; i++) {
        args[i - 1] = argv[i];
    }

    // 从stdin读取行
    while (gets(buf, sizeof(buf))) {
        // 去掉换行符
        buf[strlen(buf) - 1] = 0;

        // 添加为最后一个参数
        args[argc - 1] = buf;
        args[argc] = 0;

        if (fork() == 0) {
            // 子进程执行命令
            exec(args[0], args);
            fprintf(2, "xargs: exec %s failed\n", args[0]);
            exit(1);
        } else {
            // 父进程等待
            wait(0);
        }
    }

    exit(0);
}
