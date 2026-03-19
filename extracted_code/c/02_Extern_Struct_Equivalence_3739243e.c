/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 456
 * Language: c
 * Block ID: 3739243e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 显式对齐控制
#include <stdalign.h>

// 标准 C11 对齐方式
struct alignas(16) AlignedStruct {
    int x;
};
// alignof(AlignedStruct) = 16

// GCC/Clang 扩展
struct __attribute__((aligned(32))) GCCAligned {
    double data[4];
};

// 字段级对齐
struct FieldAlign {
    char c;
    alignas(8) int i;  // i 从 8 字节边界开始
};

//  packed 结构体 (无填充)
struct __attribute__((packed)) PackedStruct {
    char c;
    int i;  // 可能非对齐访问
};
