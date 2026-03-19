//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 559
// Language: zig
// Block ID: ef5e1a21
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() void {
    // 显式转换 (@intCast, @floatCast, etc.)
    const a: i32 = 1000;
    const b: i64 = @intCast(a);  // i32 -> i64

    const f: f64 = 3.14;
    const i: i32 = @intFromFloat(f);  // f64 -> i32 (截断)

    // 溢出检查
    const small: u8 = 255;
    const overflow_add = @addWithOverflow(small, 1);  // 返回元组 {result, overflow_bit}
    std.debug.print("Result: {}, Overflow: {}\n", .{ overflow_add[0], overflow_add[1] });

    // 饱和运算
    const sat_add = @addWithSaturation(small, 10);  // 结果为 255

    // 指针转换
    var arr = [_]u8{ 1, 2, 3, 4 };
    const ptr: [*]u8 = &arr;  // 多指针
    const slice: []u8 = ptr[0..4];  // 转切片

    // 类型擦除 (@ptrCast)
    const any_ptr: *anyopaque = @ptrCast(&arr);
    const back_to_slice: *[]u8 = @ptrCast(@alignCast(any_ptr));
}
