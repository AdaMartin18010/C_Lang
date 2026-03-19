/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\01_Type_Storage_Matrix.md
 * Line: 239
 * Language: c
 * Block ID: be6cdde8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// struct_layout.c - 结构体内存布局可视化
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

// 打印结构体布局的宏
#define PRINT_STRUCT_LAYOUT(type) do { \
    printf("\n=== %s 布局 ===\n", #type); \
    printf("总大小: %zu bytes\n", sizeof(type)); \
    printf("对齐要求: %zu bytes\n", alignof(type)); \
} while(0)

#define PRINT_MEMBER_OFFSET(type, member) \
    printf("  offsetof(%s, %s) = %zu\n", #type, #member, offsetof(type, member))

// 示例结构体
struct Simple {
    char a;    // 1 byte
    int b;     // 4 bytes
    char c;    // 1 byte
};

struct Optimized {
    int b;     // 4 bytes
    char a;    // 1 byte
    char c;    // 1 byte
};

struct Complex {
    char a;           // 1 byte
    double d;         // 8 bytes
    int i;            // 4 bytes
    short s;          // 2 bytes
    char b;           // 1 byte
};

struct ComplexOptimized {
    double d;         // 8 bytes
    int i;            // 4 bytes
    short s;          // 2 bytes
    char a;           // 1 byte
    char b;           // 1 byte
};

// 包含数组的结构体
struct WithArray {
    int id;
    char name[17];    // 17 bytes (不是4的倍数)
    double score;
};

// 嵌套结构体
struct NestedOuter {
    int a;
    struct Simple inner;
    char b;
};

// 可视化内存布局
void visualize_struct_simple(void) {
    struct Simple s = {'A', 0x12345678, 'B'};
    unsigned char *p = (unsigned char *)&s;

    printf("\n=== Simple 内存 dump (大小: %zu) ===\n", sizeof(s));
    for (size_t i = 0; i < sizeof(s); i++) {
        printf("[%02zu] 0x%02X ", i, p[i]);
        if (i == offsetof(struct Simple, a)) printf("<- a");
        if (i == offsetof(struct Simple, b)) printf("<- b[%zu]", i - offsetof(struct Simple, b));
        if (i == offsetof(struct Simple, c)) printf("<- c");
        printf("\n");
    }
}

int main(void) {
    PRINT_STRUCT_LAYOUT(struct Simple);
    PRINT_MEMBER_OFFSET(struct Simple, a);
    PRINT_MEMBER_OFFSET(struct Simple, b);
    PRINT_MEMBER_OFFSET(struct Simple, c);
    // 布局: [a:1][pad:3][b:4][c:1][pad:3] = 12 bytes

    PRINT_STRUCT_LAYOUT(struct Optimized);
    PRINT_MEMBER_OFFSET(struct Optimized, b);
    PRINT_MEMBER_OFFSET(struct Optimized, a);
    PRINT_MEMBER_OFFSET(struct Optimized, c);
    // 布局: [b:4][a:1][c:1][pad:2] = 8 bytes (节省4字节)

    PRINT_STRUCT_LAYOUT(struct Complex);
    PRINT_MEMBER_OFFSET(struct Complex, a);
    PRINT_MEMBER_OFFSET(struct Complex, d);
    PRINT_MEMBER_OFFSET(struct Complex, i);
    PRINT_MEMBER_OFFSET(struct Complex, s);
    PRINT_MEMBER_OFFSET(struct Complex, b);
    // 布局: [a:1][pad:7][d:8][i:4][s:2][b:1][pad:5] = 32 bytes

    PRINT_STRUCT_LAYOUT(struct ComplexOptimized);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, d);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, i);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, s);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, a);
    PRINT_MEMBER_OFFSET(struct ComplexOptimized, b);
    // 布局: [d:8][i:4][s:2][a:1][b:1] = 16 bytes (节省一半)

    PRINT_STRUCT_LAYOUT(struct WithArray);
    PRINT_MEMBER_OFFSET(struct WithArray, id);
    PRINT_MEMBER_OFFSET(struct WithArray, name);
    PRINT_MEMBER_OFFSET(struct WithArray, score);
    // 布局: [id:4][name:17][pad:3][score:8] = 32 bytes

    PRINT_STRUCT_LAYOUT(struct NestedOuter);
    PRINT_MEMBER_OFFSET(struct NestedOuter, a);
    PRINT_MEMBER_OFFSET(struct NestedOuter, inner);
    PRINT_MEMBER_OFFSET(struct NestedOuter, b);

    visualize_struct_simple();

    printf("\n=== 优化建议 ===\n");
    printf("1. 按成员大小降序排列结构体成员\n");
    printf("2. 使用#pragma pack谨慎压缩对齐\n");
    printf("3. 考虑使用位域减少空间占用\n");
    printf("4. 数组大小尽量对齐到自然边界\n");

    return 0;
}
