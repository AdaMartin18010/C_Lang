//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 71
// Language: zig
// Block ID: c24c3c2c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 中使用 C 整数类型
const std = @import("std");

// 导入 C 头文件
const c = @cImport({
    @cInclude("stdint.h");
    @cInclude("limits.h");
});

pub fn main() void {
    // 直接使用 C 类型
    var c_int_val: c_int = 42;
    var c_long_val: c_long = 1000000;

    // Zig 类型与 C 类型混用
    var zig_val: i32 = c_int_val;  // 隐式兼容
    std.debug.print("C int: {d}, Zig int: {d}\n", .{c_int_val, zig_val});
}
