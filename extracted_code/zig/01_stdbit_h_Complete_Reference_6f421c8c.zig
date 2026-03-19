//
// Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\01_stdbit_h_Complete_Reference.md
// Line: 137
// Language: zig
// Block ID: 6f421c8c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// Zig 内置 popcount
fn popcountExamples() void {
    const x: u32 = 0b11110000;

    // 方法 1: 内置函数
    const count1 = @popCount(x);

    // 方法 2: std.math
    const count2 = std.math.popCount(u32, x);

    // Zig 自动选择最优指令
}

// Zig 的泛型实现
fn popCountGeneric(comptime T: type, x: T) u32 {
    return @popCount(x);
}
