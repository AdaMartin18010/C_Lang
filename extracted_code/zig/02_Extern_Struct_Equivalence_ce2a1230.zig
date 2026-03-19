//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 1395
// Language: zig
// Block ID: ce2a1230
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 类型双关机制
const std = @import("std");

// Zig 没有严格别名规则，但提供了安全的类型双关原语

// 方法 1: @bitCast - 大小相同类型的位模式转换
fn bitCastExample() void {
    const f: f32 = 3.14159;
    const i: u32 = @bitCast(f);
    std.debug.print("f={d} -> bits=0x{x:0>8}\n", .{f, i});

    // 双向转换
    const f_back: f32 = @bitCast(i);
    std.debug.print("bits=0x{x:0>8} -> f={d}\n", .{i, f_back});

    // 适用于任何大小相同的类型
    const bytes: [4]u8 = @bitCast(f);
    std.debug.print("As bytes: ", .{});
    for (bytes) |b| std.debug.print("{x:0>2} ", .{b});
    std.debug.print("\n", .{});
}

// 方法 2: @ptrCast - 指针类型转换
fn ptrCastExample() void {
    var arr = [4]u8{ 0x12, 0x34, 0x56, 0x78 };

    // 字节数组转整数
    const int_ptr: *align(1) u32 = @ptrCast(&arr);
    std.debug.print("As u32: 0x{x}\n", .{int_ptr.*});

    // 注意: @ptrCast 可能需要 @alignCast
    const aligned_ptr: *u32 = @alignCast(@ptrCast(&arr));
    _ = aligned_ptr;
}

// 方法 3: std.mem.asBytes / std.mem.bytesAsValue
fn stdMemExample() void {
    const value: u32 = 0x12345678;

    // 转换为字节切片
    const bytes = std.mem.asBytes(&value);
    std.debug.print("Bytes: ", .{});
    for (bytes) |b| std.debug.print("{x:0>2} ", .{b});
    std.debug.print("\n", .{});

    // 字节切片转值
    const back: *const u32 = std.mem.bytesAsValue(u32, bytes);
    std.debug.print("Back to u32: 0x{x}\n", .{back.*});
}

// 方法 4: 使用 extern union (C 兼容)
const IntFloatUnion = extern union {
    i: i32,
    f: f32,
};

fn externUnionExample() void {
    var u: IntFloatUnion = .{ .f = 2.71828 };
    std.debug.print("f={d}, i=0x{x}\n", .{u.f, u.i});
}

// 方法 5: @ptrFromInt / @intFromPtr - 整数与指针互转
fn pointerIntExample() void {
    var value: u32 = 42;
    const ptr: *u32 = &value;

    // 指针转整数
    const addr: usize = @intFromPtr(ptr);
    std.debug.print("Pointer as int: 0x{x}\n", .{addr});

    // 整数转指针
    const ptr_back: *u32 = @ptrFromInt(addr);
    std.debug.print("Value via reconstructed pointer: {d}\n", .{ptr_back.*});
}

// 网络字节序转换 (实际应用)
fn ntohl(data: *const [4]u8) u32 {
    return @bitCast([4]u8{
        data[3], data[2], data[1], data[0],
    });
}

pub fn main() void {
    std.debug.print("=== @bitCast ===\n", .{});
    bitCastExample();

    std.debug.print("\n=== @ptrCast ===\n", .{});
    ptrCastExample();

    std.debug.print("\n=== std.mem ===\n", .{});
    stdMemExample();

    std.debug.print("\n=== extern union ===\n", .{});
    externUnionExample();

    std.debug.print("\n=== pointer/int ===\n", .{});
    pointerIntExample();

    // 网络字节序
    const network_bytes = [4]u8{ 0x12, 0x34, 0x56, 0x78 };
    std.debug.print("\n=== ntohl ===\n", .{});
    std.debug.print("Network bytes [12 34 56 78] -> 0x{x}\n", .{ntohl(&network_bytes)});
}
