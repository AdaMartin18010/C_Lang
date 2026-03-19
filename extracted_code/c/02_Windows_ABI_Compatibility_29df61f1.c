/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
 * Line: 142
 * Language: c
 * Block ID: 29df61f1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: Windows x64 整数参数传递示例
// 编译: cl.exe /FA /O2 test_int.c

#include <stdint.h>

// 前4个参数使用寄存器
void four_args(int a, int b, int c, int d) {
    // a: ECX (零扩展到 RCX)
    // b: EDX
    // c: R8D
    // d: R9D
}

// 第5个参数使用栈
void five_args(int a, int b, int c, int d, int e) {
    // a-d: ECX, EDX, R8D, R9D
    // e: [RSP+0x28] (在 Shadow Space 之后)
}

// 混合类型
void mixed_types(int64_t big, int32_t small, int16_t tiny, int8_t mini) {
    // big:   RCX
    // small: EDX (零扩展到 RDX)
    // tiny:  R8W (零扩展到 R8)
    // mini:  R9B (零扩展到 R9)
}
