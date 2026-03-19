/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\01_Lab1_Xv6_Utilities.md
 * Line: 37
 * Language: c
 * Block ID: 0a86a34a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/sleep.c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // 参数检查
    if (argc != 2) {
        fprintf(2, "Usage: sleep ticks\n");
        exit(1);
    }

    // 将字符串转换为整数
    int ticks = atoi(argv[1]);

    // 调用sleep系统调用
    sleep(ticks);

    exit(0);
}
