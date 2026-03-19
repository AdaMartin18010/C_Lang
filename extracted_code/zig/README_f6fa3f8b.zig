//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
// Line: 158
// Language: zig
// Block ID: f6fa3f8b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// zig_features.zig
const std = @import("std");
const print = std.debug.print;

// Zig: comptime - 编译期计算
const MAX_BUFFER_SIZE = 4096;

// Zig: 类型推导
fn demoTypeInference() void {
    const x: i32 = 42;
    const y = 3.14;
    const T = @TypeOf(x);
    print("Type of x: {}\n", .{@typeName(T)});
}

// Zig: 错误处理
const Error = error{ Overflow, InvalidArgument };

fn checkedAdd(a: i32, b: i32) Error!i32 {
    const result = @addWithOverflow(a, b);
    if (result[1] != 0) return Error.Overflow;
    return result[0];
}

// Zig: 可选类型
fn findItem(items: []const i32, target: i32) ?usize {
    for (items, 0..) |item, i| {
        if (item == target) return i;
    }
    return null;
}

// Zig: 编译期泛型
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

// Zig: 位操作
fn demoBitOperations() void {
    const x: u32 = 0b10110011;
    print("popcount({b}) = {}\n", .{x, @popCount(x)});
    print("ctz({b}) = {}\n", .{x, @ctz(x)});
}

// Zig: 枚举
const Color = enum(u8) {
    red, green, blue,
    pub fn toRgb(self: Color) u24 {
        return switch (self) {
            .red => 0xFF0000,
            .green => 0x00FF00,
            .blue => 0x0000FF,
        };
    }
};

// Zig: 内置测试
test "basic arithmetic" {
    const expected: i32 = 42;
    const actual = 6 * 7;
    try std.testing.expectEqual(expected, actual);
}

test "optional types" {
    const maybe_value: ?i32 = 42;
    const value = maybe_value.?;
    try std.testing.expectEqual(@as(i32, 42), value);
}

pub fn main() void {
    print("=== Zig Features Demo ===\n");
    demoTypeInference();
    demoBitOperations();

    const add_result = checkedAdd(100, 200);
    print("checkedAdd result: {}\n", .{add_result});

    const items = &[_]i32{ 10, 20, 30, 40, 50 };
    if (findItem(items, 30)) |index| {
        print("Found at index: {}\n", .{index});
    }

    const m = max(i32, 10, 20);
    print("max(10, 20) = {}\n", .{m});
}
