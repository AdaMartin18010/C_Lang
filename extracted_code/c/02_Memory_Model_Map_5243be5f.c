/*
 * Auto-generated from: 06_Thinking_Representation\03_Mind_Maps\02_Memory_Model_Map.md
 * Line: 255
 * Language: c
 * Block ID: 5243be5f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 对齐和填充示例代码 */
#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>

/* 未优化的结构体 - 存在填充 */
struct Unoptimized {
    char  c;    // 1字节, 偏移0
    // 3字节填充 (int需要4字节对齐)
    int   i;    // 4字节, 偏移4
    short s;    // 2字节, 偏移8
    // 2字节填充 (结构体总大小需是最严格对齐的倍数)
};              // 总大小: 12字节

/* 优化后的结构体 - 重新排序减少填充 */
struct Optimized {
    int   i;    // 4字节, 偏移0
    short s;    // 2字节, 偏移4
    char  c;    // 1字节, 偏移6
    // 1字节填充
};              // 总大小: 8字节

/* 紧凑结构体 - 使用packed属性 */
struct __attribute__((packed)) Packed {
    char  c;    // 1字节, 偏移0
    int   i;    // 4字节, 偏移1 (未对齐!)
    short s;    // 2字节, 偏移5
};              // 总大小: 7字节

int main(void) {
    printf("=== Alignment and Padding Demo ===\n\n");

    /* 基本类型对齐 */
    printf("Basic Type Alignments:\n");
    printf("  alignof(char)      = %zu\n", alignof(char));
    printf("  alignof(short)     = %zu\n", alignof(short));
    printf("  alignof(int)       = %zu\n", alignof(int));
    printf("  alignof(long)      = %zu\n", alignof(long));
    printf("  alignof(float)     = %zu\n", alignof(float));
    printf("  alignof(double)    = %zu\n", alignof(double));
    printf("  alignof(void*)     = %zu\n", alignof(void*));

    /* 结构体大小 */
    printf("\nStruct Sizes:\n");
    printf("  sizeof(Unoptimized) = %zu\n", sizeof(struct Unoptimized));
    printf("  sizeof(Optimized)   = %zu\n", sizeof(struct Optimized));
    printf("  sizeof(Packed)      = %zu\n", sizeof(struct Packed));

    /* 字段偏移 */
    printf("\nField Offsets (Unoptimized):\n");
    printf("  offsetof(c) = %zu\n", offsetof(struct Unoptimized, c));
    printf("  offsetof(i) = %zu\n", offsetof(struct Unoptimized, i));
    printf("  offsetof(s) = %zu\n", offsetof(struct Unoptimized, s));

    printf("\nField Offsets (Optimized):\n");
    printf("  offsetof(i) = %zu\n", offsetof(struct Optimized, i));
    printf("  offsetof(s) = %zu\n", offsetof(struct Optimized, s));
    printf("  offsetof(c) = %zu\n", offsetof(struct Optimized, c));

    /* 结构体对齐 */
    printf("\nStruct Alignments:\n");
    printf("  alignof(Unoptimized) = %zu\n", alignof(struct Unoptimized));
    printf("  alignof(Optimized)   = %zu\n", alignof(struct Optimized));
    printf("  alignof(Packed)      = %zu\n", alignof(struct Packed));

    return 0;
}
