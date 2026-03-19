//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 1430
// Language: zig
// Block ID: 0f5aaee9
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用 comptime 进行编译时优化

const std = @import("std");

/// 编译时生成的查找表
pub fn generateLookupTable(
    comptime T: type,
    comptime size: usize,
    comptime func: fn (comptime usize) T,
) [size]T {
    var table: [size]T = undefined;
    for (0..size) |i| {
        table[i] = func(i);
    }
    return table;
}

/// 编译时 CRC8 表
fn crc8Byte(comptime byte: usize) u8 {
    var crc: u8 = @intCast(byte);
    comptime var i: usize = 0;
    inline while (i < 8) : (i += 1) {
        if (crc & 0x80 != 0) {
            crc = (crc << 1) ^ 0x07;
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

const crc8_table = generateLookupTable(u8, 256, crc8Byte);

/// 运行时 CRC8 计算（使用编译时表）
pub fn crc8(data: []const u8) u8 {
    var crc: u8 = 0;
    for (data) |byte| {
        crc = crc8_table[crc ^ byte];
    }
    return crc;
}

/// 编译时字符串处理
pub fn comptimeFormat(
    comptime fmt: []const u8,
    args: anytype,
) []const u8 {
    return std.fmt.comptimePrint(fmt, args);
}

const welcome_msg = comptimeFormat(
    "Welcome to {s} v{d}.{d}.{d}!",
    .{ "MyApp", 1, 2, 3 }
);
// welcome_msg 是编译时计算的常量

/// 编译时类型反射优化
pub fn Serializer(comptime T: type) type {
    const type_info = @typeInfo(T);

    return struct {
        pub fn serialize(value: T, writer: anytype) !void {
            switch (type_info) {
                .Struct => |s| {
                    inline for (s.fields) |field| {
                        try serializeField(
                            @field(value, field.name),
                            writer
                        );
                    }
                },
                .Int => try writer.writeInt(T, value, .little),
                else => @compileError("Unsupported type"),
            }
        }

        fn serializeField(field: anytype, writer: anytype) !void {
            // 递归序列化
            try Serializer(@TypeOf(field)).serialize(field, writer);
        }
    };
}

// 编译时验证性能假设
comptime {
    // 确保查找表大小合理
    std.debug.assert(@sizeOf(@TypeOf(crc8_table)) <= 256);
}
