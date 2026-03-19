//
// Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\02_stdckdint_h_Complete_Reference.md
// Line: 226
// Language: zig
// Block ID: c22e7094
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// Zig 内置溢出检测
fn zig_overflow_detection() void {
    const a: i32 = 100;
    const b: i32 = 200;

    // 方法 1: 使用内置函数
    const sum, const overflowed = @addWithOverflow(a, b);
    if (overflowed) {
        // 处理溢出
    }

    // 方法 2: 饱和运算
    const saturated = a +| b;  // 溢出时取最大值

    // 方法 3: 回绕运算 (显式)
    const wrapped = a +% b;    // 溢出时回绕

    // 方法 4: 严格模式 (Debug 模式会 panic)
    const strict = a + b;      // 溢出时 panic (Debug)
}

// Zig 乘法检查
fn safeMultiply(a: usize, b: usize) !usize {
    const result, const overflow = @mulWithOverflow(a, b);
    if (overflow) {
        return error.Overflow;
    }
    return result;
}
