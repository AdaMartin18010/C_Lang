//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 353
// Language: zig
// Block ID: b2d6b04a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 基本函数
fn add(a: i32, b: i32) i32 {
    return a + b;
}

// 递归函数
fn factorial(n: u32) u32 {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 多返回值 (通过结构体或元组)
fn divmod(a: u32, b: u32) struct { quot: u32, rem: u32 } {
    return .{
        .quot = a / b,
        .rem = a % b,
    };
}

// 错误返回
const MyError = error{
    OutOfMemory,
    InvalidInput,
    DivisionByZero,
};

fn safe_divide(a: f64, b: f64) MyError!f64 {
    if (b == 0) return MyError.DivisionByZero;
    return a / b;
}

// 任意类型参数 (comptime)
fn print_anything(comptime T: type, value: T) void {
    std.debug.print("Value: {}\n", .{value});
}

// 可变参数 (comptime 切片)
fn sum_all(items: []const i32) i32 {
    var sum: i32 = 0;
    for (items) |item| {
        sum += item;
    }
    return sum;
}

// 内联汇编 (类似 C)
fn read_timestamp() u64 {
    var low: u32 = undefined;
    var high: u32 = undefined;
    asm volatile ("rdtsc"
        : [low] "=a" (low),
          [high] "=d" (high),
    );
    return (@as(u64, high) << 32) | low;
}

pub fn main() void {
    const result = add(5, 3);
    std.debug.print("5 + 3 = {}\n", .{result});

    const dm = divmod(17, 5);
    std.debug.print("17 / 5 = {} rem {}\n", .{ dm.quot, dm.rem });

    const div_result = safe_divide(10, 0) catch |err| {
        std.debug.print("Division failed: {}\n", .{err});
        return;
    };
    _ = div_result;
}
