//
// Auto-generated from: Zig\Zig_Learning_Paths.md
// Line: 224
// Language: zig
// Block ID: 4200d673
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 练习 2: 控制流和函数
const std = @import("std");

// 函数返回错误联合类型
fn divide(a: i32, b: i32) !i32 {
    if (b == 0) return error.DivisionByZero;
    return @divTrunc(a, b);
}

// if 作为表达式
fn max(a: i32, b: i32) i32 {
    return if (a > b) a else b;
}

// switch
fn getDayName(day: u8) []const u8 {
    return switch (day) {
        0 => "Sunday",
        1 => "Monday",
        2 => "Tuesday",
        3 => "Wednesday",
        4 => "Thursday",
        5 => "Friday",
        6 => "Saturday",
        else => "Invalid",
    };
}

pub fn main() !void {
    // 错误处理
    const result = divide(10, 2) catch |err| {
        std.debug.print("Error: {}\n", .{err});
        return;
    };
    std.debug.print("10 / 2 = {d}\n", .{result});

    // for 循环
    var sum: i32 = 0;
    for (1..11) |i| {
        sum += @intCast(i);
    }
    std.debug.print("Sum 1-10 = {d}\n", .{sum});
}
