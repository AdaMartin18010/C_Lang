//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 2756
// Language: zig
// Block ID: f26abcf3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: comptime 替换模式

// 基础泛型函数
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

fn square(x: anytype) @TypeOf(x) {
    return x * x;
}

fn arraySize(comptime T: type, arr: []const T) usize {
    return arr.len;
}

// 类型安全打印
fn printVal(value: anytype) void {
    const T = @TypeOf(value);
    switch (@typeInfo(T)) {
        .Int => std.debug.print("{d}\n", .{value}),
        .Float => std.debug.print("{d:.6}\n", .{value}),
        .Bool => std.debug.print("{}\n", .{value}),
        else => @compileError("Unsupported type for printVal"),
    }
}

// comptime 枚举生成
fn makeEnum(comptime pairs: anytype) type {
    var fields: [pairs.len]std.builtin.Type.EnumField = undefined;
    inline for (pairs, 0..) |pair, i| {
        fields[i] = .{
            .name = pair[0],
            .value = pair[1],
        };
    }
    return @Type(.{
        .Enum = .{
            .tag_type = u32,
            .fields = &fields,
            .decls = &.{},
            .is_exhaustive = true,
        },
    });
}

const Color = makeEnum(&.{
    .{ "red", 0xFF0000 },
    .{ "green", 0x00FF00 },
    .{ "blue", 0x0000FF },
});

// 使用
// Color.red, Color.green, Color.blue
