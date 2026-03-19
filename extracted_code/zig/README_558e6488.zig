//
// Auto-generated from: Zig\README.md
// Line: 1092
// Language: zig
// Block ID: 558e6488
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 编译时计算阶乘
fn factorial(comptime n: u32) u32 {
    var result: u32 = 1;
    comptime var i = 1;
    inline while (i <= n) : (i += 1) {
        result *= i;
    }
    return result;
}

// 编译时已知值
const fact5 = factorial(5); // 在编译期计算为 120

pub fn main() void {
    std.debug.print("5! = {d}\n", .{fact5});
}
