//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 2437
// Language: zig
// Block ID: caf60633
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 查找表生成
const std = @import("std");

// CRC8 查找表（编译时生成）
const CRC8_TABLE = comptime blk: {
    var table: [256]u8 = undefined;
    for (0..256) |i| {
        var crc: u8 = @intCast(i);
        var j: u8 = 0;
        while (j < 8) : (j += 1) {
            crc = if (crc & 0x80 != 0)
                (crc << 1) ^ 0x07
            else
                crc << 1;
        }
        table[i] = crc;
    }
    break :blk table;
};

// 正弦查找表（编译时生成）
fn SinTable(comptime size: usize) [size]f32 {
    var table: [size]f32 = undefined;
    for (0..size) |i| {
        const angle = @as(f32, @floatFromInt(i)) * 2.0 * std.math.pi / @as(f32, @floatFromInt(size));
        table[i] = @sin(angle);
    }
    return table;
}

const SIN_256 = SinTable(256);
const SIN_1024 = SinTable(1024);

// 对数查找表（编译时生成，指定精度）
fn LogTable(comptime input_bits: u32, comptime output_bits: u32) [1 << input_bits]u32 {
    const table_size = 1 << input_bits;
    var table: [table_size]u32 = undefined;
    for (0..table_size) |i| {
        if (i == 0) {
            table[i] = 0;  // log(0) 未定义，设为 0
        } else {
            const x = @as(f32, @floatFromInt(i)) / @as(f32, @floatFromInt(table_size));
            const log_val = @log2(x);
            // 量化到输出位宽
            table[i] = @intFromFloat(log_val * @as(f32, @floatFromInt(1 << (output_bits - 1))));
        }
    }
    return table;
}

const LOG_TABLE_8_16 = LogTable(8, 16);  // 256 entries, 16-bit output

// 使用查找表
fn fast_crc8(data: []const u8) u8 {
    var crc: u8 = 0;
    for (data) |byte| {
        crc = CRC8_TABLE[crc ^ byte];
    }
    return crc;
}

fn fast_sin(angle: u8) f32 {
    return SIN_256[angle];  // angle 是 0-255 表示 0-2π
}
