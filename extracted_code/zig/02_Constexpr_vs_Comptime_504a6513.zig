//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 627
// Language: zig
// Block ID: 504a6513
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig comptime 函数详解
const std = @import("std");

// ========== 基本 comptime 函数 ==========

// 纯计算函数
fn add(comptime a: i32, comptime b: i32) i32 {
    return a + b;
}

// 多语句函数
fn gcd(comptime a: u32, comptime b: u32) u32 {
    var x = a;
    var y = b;
    while (y != 0) {
        const temp = y;
        y = x % y;
        x = temp;
    }
    return x;
}

// 递归函数
fn factorial(comptime n: u32) u32 {
    return if (n <= 1) 1 else n * factorial(n - 1);
}

// ========== 类型参数函数 ==========

// 类型构造函数
fn Vector(comptime n: usize, comptime T: type) type {
    return struct {
        data: [n]T,

        const Self = @This();

        fn add(self: Self, other: Self) Self {
            var result: Self = undefined;
            for (0..n) |i| {
                result.data[i] = self.data[i] + other.data[i];
            }
            return result;
        }

        fn dot(self: Self, other: Self) T {
            var sum: T = 0;
            for (0..n) |i| {
                sum += self.data[i] * other.data[i];
            }
            return sum;
        }
    };
}

// ========== comptime 代码块 ==========

// 复杂编译时计算
const TABLE_SIZE = comptime blk: {
    var size: usize = 1;
    var i: usize = 1;
    while (i <= 5) : (i += 1) {
        size *= i;
    }
    break :blk size;
};  // 120

// 编译时查找表生成
const SIN_TABLE = comptime blk: {
    var table: [360]f32 = undefined;
    for (0..360) |i| {
        const rad = @as(f32, @floatFromInt(i)) * std.math.pi / 180.0;
        table[i] = @sin(rad);
    }
    break :blk table;
};

// ========== 编译时验证 ==========

// 类型约束检查
fn checked_array(comptime T: type, comptime size: usize) type {
    comptime {
        if (size == 0) {
            @compileError("Array size must be greater than 0");
        }
        if (@sizeOf(T) == 0) {
            @compileError("Zero-sized types not allowed");
        }
    }
    return struct {
        data: [size]T,
        len: usize = size,
    };
}

// 编译时断言
fn assert_power_of_two(comptime n: usize) void {
    comptime {
        if (n & (n - 1) != 0) {
            @compileError("Value must be power of two");
        }
    }
}

const PAGE_SIZE = 4096;
comptime assert_power_of_two(PAGE_SIZE);  // OK

// const BAD_SIZE = 1000;
// comptime assert_power_of_two(BAD_SIZE);  // 编译错误

// ========== 混合运行时/编译时 ==========

// 函数可以接受运行时和编译时参数
fn compute(comptime multiplier: i32, value: i32) i32 {
    return value * multiplier;  // multiplier 在编译时已知，可优化
}

// 编译时分支
fn optimized_sqrt(comptime T: type, value: T) T {
    comptime {
        if (@typeInfo(T) == .Float) {
            // 浮点专用路径
            return @sqrt(value);
        } else {
            @compileError("sqrt only for float types");
        }
    }
}
