/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
 * Line: 470
 * Language: c
 * Block ID: 03ece1ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: 整数类型传递示例

#include <stdint.h>

// 基本整数类型
void basic_ints(char c, short s, int i, long l) {
    // c: %dil (8位, 提升)
    // s: %si (16位, 提升)
    // i: %edx (32位)
    // l: %rcx (64位)
}

// 固定宽度整数
void fixed_width(int8_t i8, uint16_t u16, int32_t i32, uint64_t u64) {
    // i8:  %dil (提升)
    // u16: %si (提升)
    // i32: %edx
    // u64: %rcx
}

// 128位整数 (GCC/Clang 扩展)
void int128(__int128 x) {
    // x 低位: %rdi
    // x 高位: %rsi
}

// 多个整数 (超过寄存器限制)
void many_ints(int a, int b, int c, int d, int e, int f, int g) {
    // a: %edi
    // b: %esi
    // c: %edx
    // d: %ecx
    // e: %r8d
    // f: %r9d
    // g: 8(%rsp)  <-- 第7个参数，使用栈
}
