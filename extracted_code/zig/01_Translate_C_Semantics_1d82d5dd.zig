//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1414
// Language: zig
// Block ID: 1d82d5dd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码: simple_math.zig
// 注意: 实际 translate-c 输出可能略有不同

pub const PI = 3.141592653589793;
pub const E = 2.718281828459045;
pub const MAX_ITERATIONS = @as(c_int, 1000);
pub const EPSILON = 1.0e-10;

pub const MathError = c_int;
pub const MATH_OK: MathError = 0;
pub const MATH_ERROR_INVALID_INPUT: MathError = -1;
pub const MATH_ERROR_DIV_BY_ZERO: MathError = -2;
pub const MATH_ERROR_OVERFLOW: MathError = -3;

pub const Vec3 = extern struct {
    x: f64,
    y: f64,
    z: f64,
};

pub const MathContext = opaque {};

pub extern fn vec3_add(a: [*c]const Vec3, b: [*c]const Vec3, result: [*c]Vec3) MathError;
pub extern fn vec3_scale(v: [*c]const Vec3, factor: f64, result: [*c]Vec3) MathError;
pub extern fn vec3_length(v: [*c]const Vec3) f64;

pub extern fn math_context_create() ?*MathContext;
pub extern fn math_context_destroy(ctx: ?*MathContext) void;
pub extern fn math_solve_quadratic(
    a: f64,
    b: f64,
    c: f64,
    root1: [*c]f64,
    root2: [*c]f64
) MathError;

pub const MathProgressCallback = ?*const fn (c_int, c_int, ?*anyopaque) callconv(.C) void;

pub extern fn math_iterate(
    callback: MathProgressCallback,
    user_data: ?*anyopaque
) MathError;
