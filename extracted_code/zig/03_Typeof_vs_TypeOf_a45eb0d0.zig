//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 2163
// Language: zig
// Block ID: a45eb0d0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 泛型数学函数

const std = @import("std");

// 泛型绝对值
fn abs(x: anytype) @TypeOf(x) {
    return if (x < 0) -x else x;
}

// 泛型平方
fn square(x: anytype) @TypeOf(x) {
    return x * x;
}

// 泛型限制
fn clamp(x: anytype, min: @TypeOf(x), max: @TypeOf(x)) @TypeOf(x) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// 泛型线性插值
fn lerp(comptime T: type, a: T, b: T, t: f32) T {
    return a + (b - a) * @as(T, @floatCast(t));
}

// 使用 comptime 验证数值类型
fn validateNumeric(comptime T: type) void {
    comptime {
        const info = @typeInfo(T);
        switch (info) {
            .Int, .Float, .ComptimeInt, .ComptimeFloat => {},
            else => @compileError("Type must be numeric"),
        }
    }
}

fn addSafe(comptime T: type, a: T, b: T) T {
    validateNumeric(T);
    return a + b;
}

// 使用示例
fn demoGenericMath() void {
    const a = abs(-42);
    const s = square(5);
    const c = clamp(150, 0, 100);
    const l = lerp(f32, 0.0, 10.0, 0.5);

    std.debug.print("abs: {}, square: {}, clamp: {}, lerp: {}\n", .{
        a, s, c, l,
    });
}
