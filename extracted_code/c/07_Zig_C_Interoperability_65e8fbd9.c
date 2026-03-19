/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
 * Line: 509
 * Language: c
 * Block ID: 65e8fbd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c
#include <stdio.h>
#include "zigmath.h"

int main() {
    // 调用Zig函数
    int sum = zig_add(10, 20);
    printf("Sum: %d\n", sum);

    // 使用错误处理
    int result;
    int err = zig_divide(100, 0, &result);
    if (err != 0) {
        printf("Division by zero!\n");
    }

    err = zig_divide(100, 3, &result);
    if (err == 0) {
        printf("100 / 3 = %d\n", result);
    }

    // 使用Zig结构体
    Vector v = create_vector(3.0f, 4.0f, 0.0f);
    float len = vector_length(v);
    printf("Vector length: %.2f\n", len);

    return 0;
}
