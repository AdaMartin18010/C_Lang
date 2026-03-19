//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
// Line: 171
// Language: zig
// Block ID: 128c5d90
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 对应的 Windows x64 函数声明

// 前4个参数使用寄存器
extern "C" fn fourArgs(a: c_int, b: c_int, c: c_int, d: c_int) void;
// 与 C 代码完全相同的寄存器分配

// 第5个参数使用栈
extern "C" fn fiveArgs(a: c_int, b: c_int, c: c_int, d: c_int, e: c_int) void;
// a-d: RCX, RDX, R8, R9
// e: [RSP+0x28]

// 混合类型
extern "C" fn mixedTypes(big: i64, small: i32, tiny: i16, mini: i8) void;
// 与 C 代码完全相同的寄存器分配
