/*
 * Auto-generated from: Zig\README.md
 * Line: 914
 * Language: c
 * Block ID: 2ce2cb0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c
#include <stdio.h>
#include "mylib.h"

int main() {
    // 调用 Zig 函数
    int result = zig_add(10, 20);
    printf("10 + 20 = %d\n", result);

    // 使用 Zig 结构体
    ZigPoint p = {3.0, 4.0};
    double dist = zig_distance(&p);
    printf("Distance = %f\n", dist);

    return 0;
}
