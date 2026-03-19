//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 713
// Language: zig
// Block ID: 298dfa9b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// comptime 是 Zig 最强大的特性
// 在编译期执行代码

// 编译时常量
const array_len = comptime blk: {
    var sum: usize = 0;
    for (0..100) |i| {
        sum += i;
    }
    break :blk sum;  // 4950
};

// 编译时函数
fn fibonacci(comptime n: u32) u32 {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

const fib_10 = comptime fibonacci(10);  // 55，编译期计算

// 类型作为参数
fn max(comptime T: type, a: T, b: T) T {
    return if (a > b) a else b;
}

// 生成类型
fn Vector(comptime n: usize, comptime T: type) type {
    return struct {
        data: [n]T,

        fn add(self: @This(), other: @This()) @This() {
            var result: @This() = undefined;
            for (0..n) |i| {
                result.data[i] = self.data[i] + other.data[i];
            }
            return result;
        }

        fn dot(self: @This(), other: @This()) T {
            var sum: T = 0;
            for (0..n) |i| {
                sum += self.data[i] * other.data[i];
            }
            return sum;
        }
    };
}

pub fn main() void {
    // 使用 comptime 生成的类型
    const Vec3f = Vector(3, f32);

    const v1 = Vec3f{ .data = .{ 1, 2, 3 } };
    const v2 = Vec3f{ .data = .{ 4, 5, 6 } };

    const sum = v1.add(v2);
    std.debug.print("Sum: {}\n", .{sum.data});

    const dot_product = v1.dot(v2);  // 1*4 + 2*5 + 3*6 = 32
    std.debug.print("Dot: {}\n", .{dot_product});

    // 编译时类型检查
    const is_int = comptime std.meta.trait.isIntegral(i32);
    std.debug.print("i32 is integral: {}\n", .{is_int});
}
