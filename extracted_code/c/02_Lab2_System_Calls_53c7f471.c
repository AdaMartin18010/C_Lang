/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 185
 * Language: c
 * Block ID: 53c7f471
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/user.h
int trace(int);

// user/usys.pl
entry("trace");

// user/trace.c (测试程序)
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int i;
    char *nargv[MAXARG];

    if (argc < 3) {
        fprintf(2, "Usage: trace mask command [args...]\n");
        exit(1);
    }

    // 设置跟踪掩码
    if (trace(atoi(argv[1])) < 0) {
        fprintf(2, "trace: trace failed\n");
        exit(1);
    }

    // 执行命令
    for (i = 2; i < argc && i < MAXARG; i++)
        nargv[i-2] = argv[i];
    nargv[argc-2] = 0;

    exec(nargv[0], nargv);

    fprintf(2, "trace: exec %s failed\n", nargv[0]);
    exit(1);
}
