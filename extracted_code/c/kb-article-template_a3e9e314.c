/*
 * Auto-generated from: templates\kb-article-template.md
 * Line: 192
 * Language: c
 * Block ID: a3e9e314
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 推荐的代码风格
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// 函数注释说明用途
int32_t add(int32_t a, int32_t b) {
    // 边界检查
    if (b > 0 && a > INT32_MAX - b) {
        return INT32_MAX;  // 溢出处理
    }
    return a + b;
}

int main(void) {
    int32_t result = add(10, 20);
    printf("Result: %d\n", result);
    return 0;
}
