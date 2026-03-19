/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 769
 * Language: c
 * Block ID: 641b9c83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c
#include "vector.h"
#include <stdio.h>

int main(void) {
    // 创建int向量
    auto v = vector_create(int);

    // 添加元素
    vector_push(&v, 10);
    vector_push(&v, 20);
    vector_push(&v, 30);

    // 访问元素
    for (size_t i = 0; i < v.size; i++) {
        printf("v[%zu] = %d\n", i, vector_get(&v, int, i));
    }

    // 使用范围for (C23可能支持)
    // for (auto it : v) { ... }

    vector_destroy(&v);
    return 0;
}
