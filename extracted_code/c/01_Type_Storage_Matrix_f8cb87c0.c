/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 142
 * Language: c
 * Block ID: f8cb87c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// alignment.c - 类型对齐查询
#include <stdio.h>
#include <stdalign.h>

// C11方式获取对齐值
#define PRINT_ALIGNMENT(type) \
    printf("alignof(%-12s) = %zu\n", #type, alignof(type))

// 传统方式: 使用offsetof技巧
#define ALIGNOF_TYPE(type) \
    offsetof(struct { char c; type member; }, member)

int main(void) {
    printf("=== C11 alignof 值 ===\n");
    PRINT_ALIGNMENT(char);
    PRINT_ALIGNMENT(short);
    PRINT_ALIGNMENT(int);
    PRINT_ALIGNMENT(long);
    PRINT_ALIGNMENT(long long);
    PRINT_ALIGNMENT(float);
    PRINT_ALIGNMENT(double);
    PRINT_ALIGNMENT(long double);
    PRINT_ALIGNMENT(void*);

    return 0;
}
