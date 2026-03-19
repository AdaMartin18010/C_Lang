//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 1690
// Language: zig
// Block ID: cd627f9f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: mathlib.zig - Zig 绑定

const std = @import("std");

// 基本数学函数
extern fn factorial(n: i32) i64;
extern fn power(base: f64, exp: i32) f64;

// 向量类型 (必须与 C 布局兼容)
pub const Vec3 = extern struct {
    x: f64,
    y: f64,
    z: f64,
};

extern fn vec3_add(a: Vec3, b: Vec3) Vec3;
extern fn vec3_dot(a: Vec3, b: Vec3) f64;
extern fn vec3_scale(v: Vec3, s: f64) Vec3;

// 回调类型
pub const UnaryOp = *const fn (f64) callconv(.C) f64;
extern fn map_array(arr: [*]f64, n: usize, op: UnaryOp) void;

// 错误枚举
pub const MathError = enum(c_int) {
    ok = 0,
    domain_error = 1,
    range_error = 2,
};

pub const MathResult = extern struct {
    value: f64,
    error: MathError,
};

extern fn safe_sqrt(x: f64) MathResult;

// Zig 友好的包装函数
pub fn vec3Add(a: Vec3, b: Vec3) Vec3 {
    return vec3_add(a, b);
}

pub fn vec3Dot(a: Vec3, b: Vec3) f64 {
    return vec3_dot(a, b);
}

pub fn safeSqrt(x: f64) !f64 {
    const result = safe_sqrt(x);
    return switch (result.error) {
        .ok => result.value,
        .domain_error => error.DomainError,
        .range_error => error.RangeError,
    };
}

// 使用示例
test "vec3 operations" {
    const a = Vec3{ .x = 1, .y = 2, .z = 3 };
    const b = Vec3{ .x = 4, .y = 5, .z = 6 };

    const sum = vec3Add(a, b);
    try std.testing.expectEqual(@as(f64, 5), sum.x);
    try std.testing.expectEqual(@as(f64, 7), sum.y);
    try std.testing.expectEqual(@as(f64, 9), sum.z);

    const dot = vec3Dot(a, b);
    try std.testing.expectEqual(@as(f64, 32), dot); // 1*4 + 2*5 + 3*6
}
