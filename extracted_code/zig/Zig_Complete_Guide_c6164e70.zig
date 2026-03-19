//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 785
// Language: zig
// Block ID: c6164e70
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 生成查找表
const sin_table = comptime blk: {
    var table: [360]f32 = undefined;
    for (0..360) |i| {
        const rad = @as(f32, @floatFromInt(i)) * std.math.pi / 180.0;
        table[i] = @sin(rad);
    }
    break :blk table;
};

// 自动生成枚举和字符串映射
fn makeEnumAndStringMap(comptime names: []const []const u8) type {
    var enum_fields: [names.len]std.builtin.Type.EnumField = undefined;
    var map_entries: [names.len]struct { []const u8, usize } = undefined;

    for (names, 0..) |name, i| {
        enum_fields[i] = .{
            .name = name,
            .value = i,
        };
        map_entries[i] = .{ name, i };
    }

    return struct {
        pub const Enum = @Type(.{
            .Enum = .{
                .tag_type = u32,
                .fields = &enum_fields,
                .decls = &.{},
                .is_exhaustive = true,
            },
        });

        pub const map = std.StaticStringMap(usize).initComptime(&map_entries);
    };
}

const ColorSet = makeEnumAndStringMap(&.{ "red", "green", "blue", "yellow" });
const Color = ColorSet.Enum;

pub fn main() void {
    std.debug.print("Sin(30°) = {}\n", .{sin_table[30]});

    const c = Color.green;
    std.debug.print("Color: {}\n", .{@intFromEnum(c)});

    if (ColorSet.map.get("blue")) |idx| {
        std.debug.print("Blue index: {}\n", .{idx});
    }
}
