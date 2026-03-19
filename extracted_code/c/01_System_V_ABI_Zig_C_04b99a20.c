/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 1763
 * Language: c
 * Block ID: 04b99a20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 对齐控制

#include <stdalign.h>
#include <stdint.h>

// 默认对齐的结构体
struct DefaultAlign {
    char c;
    int i;
};
// sizeof = 8, alignof = 4

// 显式对齐
struct ExplicitAlign {
    char c;
    alignas(16) int i;
};
// alignof = 16

// packed 结构体 (无填充)
struct __attribute__((packed)) PackedStruct {
    char c;
    int i;
};
// sizeof = 5, alignof = 1

// 对齐的缓冲区
alignas(64) char cache_line_buffer[64];
// 适合 SIMD 操作，避免伪共享
