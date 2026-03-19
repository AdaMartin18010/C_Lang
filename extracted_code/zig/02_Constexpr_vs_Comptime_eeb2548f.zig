//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 2177
// Language: zig
// Block ID: eeb2548f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 编译时数学计算
const std = @import("std");

// 编译时阶乘
fn factorial(comptime n: u64) u64 {
    return if (n <= 1) 1 else n * factorial(n - 1);
}

// 编译时幂运算
fn power(comptime base: f64, comptime exp: i32) f64 {
    return if (exp == 0)
        1.0
    else if (exp < 0)
        1.0 / power(base, -exp)
    else
        base * power(base, exp - 1);
}

// 编译时平方根
fn sqrtNewtown(comptime x: f64, comptime guess: f64, comptime iter: u32) f64 {
    return if (iter == 0)
        guess
    else
        sqrtNewtown(x, (guess + x / guess) / 2.0, iter - 1);
}

fn sqrtCt(comptime x: f64) f64 {
    return sqrtNewtown(x, x / 2.0, 20);
}

// 使用
const PI = std.math.pi;
const SQRT_2 = sqrtCt(2.0);
const FACT_20 = factorial(20);

// 完整查找表生成
const SIN_TABLE = comptime blk: {
    var table: [360]f32 = undefined;
    for (0..360) |i| {
        const rad = @as(f32, @floatFromInt(i)) * std.math.pi / 180.0;
        table[i] = @sin(rad);
    }
    break :blk table;
};

// 使用查找表
fn fastSin(degrees: u32) f32 {
    return SIN_TABLE[degrees % 360];
}
