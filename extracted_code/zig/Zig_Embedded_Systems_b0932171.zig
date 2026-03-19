//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 331
// Language: zig
// Block ID: b0932171
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// mmio.zig - 内存映射 I/O 工具

const std = @import("std");

// 寄存器读写
pub fn read_volatile(comptime T: type, address: usize) T {
    return @as(*volatile T, @ptrFromInt(address)).*;
}

pub fn write_volatile(comptime T: type, address: usize, value: T) void {
    @as(*volatile T, @ptrFromInt(address)).* = value;
}

// 修改寄存器字段
pub fn modify_volatile(comptime T: type, address: usize, mask: T, value: T) void {
    const current = read_volatile(T, address);
    write_volatile(T, address, (current & ~mask) | (value & mask));
}

// 位操作宏
pub fn bit(comptime n: u5) u32 {
    return @as(u32, 1) << n;
}

pub fn bits(comptime msb: u5, comptime lsb: u5) u32 {
    return ((@as(u32, 1) << (msb - lsb + 1)) - 1) << lsb;
}
