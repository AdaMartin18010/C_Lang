//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 283
// Language: zig
// Block ID: d731f3e6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() void {
    // if-else (表达式，不是语句！)
    const x: i32 = 10;
    const message = if (x > 0) "positive" else if (x < 0) "negative" else "zero";

    // while 循环
    var i: u32 = 0;
    while (i < 10) : (i += 1) {
        std.debug.print("{} ", .{i});
    }
    std.debug.print("\n", .{});

    // while 带条件
    var optional_num: ?u32 = 5;
    while (optional_num) |num| : (optional_num = if (num > 0) num - 1 else null) {
        std.debug.print("{} ", .{num});
    }
    std.debug.print("\n", .{});

    // for 循环 (用于数组/切片)
    const arr = [_]u32{ 1, 2, 3, 4, 5 };
    for (arr) |item, index| {
        std.debug.print("[{}] = {}\n", .{ index, item });
    }

    // for 带指针 (可修改)
    var mutable_arr = [_]u32{ 1, 2, 3, 4, 5 };
    for (&mutable_arr) |*item| {
        item.* *= 2;  // 解引用指针并修改
    }

    // switch (穷尽匹配)
    const n: u8 = 5;
    const description = switch (n) {
        0 => "zero",
        1...9 => "single digit",  // 范围
        10 => "ten",
        else => "other",
    };

    // switch 带枚举
    const Color = enum { red, green, blue };
    const color = Color.green;
    const color_name = switch (color) {
        .red => "Red",
        .green => "Green",
        .blue => "Blue",
    };

    // defer (函数退出时执行)
    defer std.debug.print("Function ending\n", .{});
    std.debug.print("Function running\n", .{});

    // errdefer (只有错误时才执行)
    may_fail() catch |err| {
        std.debug.print("Error: {}\n", .{err});
    };
}

fn may_fail() !void {
    errdefer std.debug.print("Cleaning up after error\n", .{});
    return error.SomeError;
}
