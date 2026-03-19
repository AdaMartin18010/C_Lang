//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 972
// Language: zig
// Block ID: cca7b7bd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() void {
    var x: i32 = 42;

    // 单指针 (*T)
    const ptr: *i32 = &x;
    ptr.* = 100;  // 解引用
    std.debug.print("x = {}\n", .{x});  // 100

    // 切片 ([]T) - 胖指针：指针+长度
    var arr = [_]u32{ 1, 2, 3, 4, 5 };
    const slice = arr[1..4];  // { 2, 3, 4 }
    std.debug.print("Slice len: {}, first: {}\n", .{ slice.len, slice[0] });

    // 多指针 ([*]T) - 类似 C 指针
    const many_ptr: [*]u8 = @ptrCast(&arr);
    std.debug.print("First via many ptr: {}\n", .{many_ptr[0]});

    // 哨兵指针 ([*:0]T) - 以哨兵值终止
    const c_string: [*:0]const u8 = "hello";
    _ = c_string;

    // 可选指针 (?*T)
    const maybe_ptr: ?*i32 = &x;
    if (maybe_ptr) |p| {
        std.debug.print("Pointer value: {}\n", .{p.*});
    }

    // 任意指针 (*anyopaque, ?*anyopaque)
    const any_ptr: *anyopaque = @ptrCast(&x);
    const back_to_i32: *i32 = @ptrCast(@alignCast(any_ptr));
    std.debug.print("Via opaque: {}\n", .{back_to_i32.*});
}

// 指针运算
fn pointer_arithmetic() void {
    var arr = [_]u32{ 10, 20, 30, 40, 50 };
    var ptr: [*]u32 = &arr;

    ptr += 2;  // 指向 arr[2]
    std.debug.print("Value at ptr: {}\n", .{ptr[0]});  // 30
}

// 对齐
fn alignment() void {
    // 检查对齐
    const ptr: *u32 = @ptrFromInt(0x1004);
    const is_aligned = @intFromPtr(ptr) % @alignOf(u32) == 0;
    std.debug.print("Is aligned: {}\n", .{is_aligned});

    // 对齐指针
    var unaligned_ptr: usize = 0x1001;
    const aligned = std.mem.alignForward(usize, unaligned_ptr, @alignOf(u32));
    std.debug.print("Aligned: 0x{x}\n", .{aligned});  // 0x1004
}
