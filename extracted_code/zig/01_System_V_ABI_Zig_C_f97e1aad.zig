//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 509
// Language: zig
// Block ID: f97e1aad
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 对应的整数传递

// 基本整数类型
extern fn basicInts(c: u8, s: i16, i: i32, l: i64) void {
    // c: %dil
    // s: %si
    // i: %edx
    // l: %rcx
}

// 固定宽度整数
extern fn fixedWidth(i8: i8, u16: u16, i32: i32, u64: u64) void {
    // 与 C 代码相同的寄存器分配
}

// 128位整数
extern fn int128(x: i128) void {
    // x 低位: %rdi
    // x 高位: %rsi
}

// 多个整数
extern fn manyInts(a: i32, b: i32, c: i32, d: i32,
                   e: i32, f: i32, g: i32) void {
    // a-f: %edi, %esi, %edx, %ecx, %r8d, %r9d
    // g: 8(%rsp)
}

// Zig 特有的编译时整数
fn comptimeInt(x: comptime_int) void {
    // 编译时已知值，不占用运行时寄存器
    const value = x;  // 直接内联
}
