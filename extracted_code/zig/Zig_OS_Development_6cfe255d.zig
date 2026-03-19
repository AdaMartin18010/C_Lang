//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 1058
// Language: zig
// Block ID: 6cfe255d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// serial.zig - 串口输出
const std = @import("std");

const PORT = 0x3F8; // COM1

pub fn init() void {
    @as(*volatile u8, @ptrFromInt(PORT + 1)).* = 0x00; // 禁用中断
    @as(*volatile u8, @ptrFromInt(PORT + 3)).* = 0x80; // 启用 DLAB
    @as(*volatile u8, @ptrFromInt(PORT + 0)).* = 0x03; // 波特率 38400
    @as(*volatile u8, @ptrFromInt(PORT + 1)).* = 0x00;
    @as(*volatile u8, @ptrFromInt(PORT + 3)).* = 0x03; // 8N1
    @as(*volatile u8, @ptrFromInt(PORT + 2)).* = 0xC7; // FIFO
}

pub fn write(ch: u8) void {
    while ((@as(*volatile u8, @ptrFromInt(PORT + 5)).* & 0x20) == 0) {}
    @as(*volatile u8, @ptrFromInt(PORT)).* = ch;
}

pub fn print(s: []const u8) void {
    for (s) |ch| {
        write(ch);
    }
}
