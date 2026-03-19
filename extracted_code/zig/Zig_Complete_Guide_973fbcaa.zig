//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 241
// Language: zig
// Block ID: 973fbcaa
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() void {
    // const: 编译时常量，不可修改
    const pi: f64 = 3.14159;
    const greeting = "Hello";  // 类型自动推断为 []const u8

    // var: 可变变量
    var count: u32 = 0;
    count += 1;

    // 块级作用域和标签
    const result = blk: {
        var sum: u32 = 0;
        var i: u32 = 0;
        while (i < 10) : (i += 1) {
            sum += i;
        }
        break :blk sum;  // 块的返回值
    };
    std.debug.print("Sum: {}\n", .{result});  // 45

    // 可选类型 (?T)
    var maybe_value: ?i32 = null;
    maybe_value = 42;

    // if 捕获可选值
    if (maybe_value) |value| {
        std.debug.print("Value: {}\n", .{value});
    } else {
        std.debug.print("No value\n", .{});
    }

    // 错误联合类型 (T!E)
    var maybe_error: !i32 = 42;  // 可以是 i32 或错误
    maybe_error = error.OutOfMemory;
}
