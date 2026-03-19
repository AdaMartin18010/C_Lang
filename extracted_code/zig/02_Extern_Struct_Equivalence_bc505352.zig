//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 1666
// Language: zig
// Block ID: bc505352
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 跨平台策略
const std = @import("std");

// 策略 1: 使用 Zig 的 fixed-width 类型
const PortableHeader = extern struct {
    magic: u32,      // 固定 4 字节
    version: u32,    // 固定 4 字节
    timestamp: u64,  // 固定 8 字节
    data_size: u64,  // 固定 8 字节
};

// 编译时验证
comptime {
    std.debug.assert(@sizeOf(PortableHeader) == 24);
    std.debug.assert(@alignOf(PortableHeader) == 8);
}

// 策略 2: 使用 packed struct 保证位级兼容
const NetworkPacket = packed struct {
    type: u8,
    length: u16,
    seq: u32,
};

comptime {
    std.debug.assert(@sizeOf(NetworkPacket) == 7);
    std.debug.assert(@alignOf(NetworkPacket) == 1);
}

// 策略 3: 显式字节序处理
fn writeU32Be(buf: *[4]u8, value: u32) void {
    buf[0] = @truncate(value >> 24);
    buf[1] = @truncate(value >> 16);
    buf[2] = @truncate(value >> 8);
    buf[3] = @truncate(value);
}

fn readU32Be(buf: *const [4]u32) u32 {
    return (@as(u32, buf[0]) << 24) |
           (@as(u32, buf[1]) << 16) |
           (@as(u32, buf[2]) << 8)  |
           @as(u32, buf[3]);
}

// 策略 4: 使用标准库序列化
fn serializeHeader(writer: anytype, h: PortableHeader) !void {
    try writer.writeInt(u32, h.magic, .big);
    try writer.writeInt(u32, h.version, .big);
    try writer.writeInt(u64, h.timestamp, .big);
    try writer.writeInt(u64, h.data_size, .big);
}

// 策略 5: 目标平台检测
const Target = struct {
    const is_little_endian = @import("builtin").target.cpu.arch.endian() == .little;
    const word_size = @sizeOf(usize);

    fn describe() void {
        std.debug.print("Target: {s}-endian, {d}-bit\n", .{
            if (is_little_endian) "little" else "big",
            word_size * 8,
        });
    }
};

pub fn main() void {
    Target.describe();

    std.debug.print("PortableHeader: size={d}, align={d}\n", .{
        @sizeOf(PortableHeader),
        @alignOf(PortableHeader),
    });

    std.debug.print("NetworkPacket: size={d}, align={d}\n", .{
        @sizeOf(NetworkPacket),
        @alignOf(NetworkPacket),
    });
}
