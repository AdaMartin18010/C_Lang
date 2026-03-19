//
// Auto-generated from: Zig\Zig_Learning_Paths.md
// Line: 170
// Language: zig
// Block ID: 84c2a3fe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 练习 1: 变量和类型
const std = @import("std");

pub fn main() void {
    // 常量必须初始化
    const pi: f64 = 3.14159;

    // 变量可以修改
    var count: i32 = 0;
    count += 1;

    // 类型推断
    var name = "Zig";  // 推断为 []const u8

    std.debug.print("pi = {d}, count = {d}, name = {s}\n",
        .{pi, count, name});
}
