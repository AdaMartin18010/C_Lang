/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 129
 * Language: c
 * Block ID: 17d995df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 标准结构体布局规则
#include <stddef.h>
#include <stdio.h>

struct Example {
    char c;      // 1 byte
    int i;       // 4 bytes (通常)
    short s;     // 2 bytes
};

// 内存布局分析 (典型 32/64 位平台):
// offset 0:  char c    [1 byte]
// offset 1:  padding   [3 bytes]  <-- 为 int 对齐到 4 字节边界
// offset 4:  int i     [4 bytes]
// offset 8:  short s   [2 bytes]
// offset 10: padding   [2 bytes]  <-- 结构体总大小对齐到最大成员
// Total: 12 bytes

int main() {
    printf("sizeof(struct Example) = %zu\n", sizeof(struct Example));
    printf("offsetof(c) = %zu\n", offsetof(struct Example, c));
    printf("offsetof(i) = %zu\n", offsetof(struct Example, i));
    printf("offsetof(s) = %zu\n", offsetof(struct Example, s));
    return 0;
}
