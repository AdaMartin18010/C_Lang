//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 102
// Language: zig
// Block ID: 3259bd11
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// math.zig

const std = @import("std");
const testing = std.testing;

// 被测试的函数
pub fn add(a: i32, b: i32) i32 {
    return a + b;
}

pub fn factorial(n: u32) !u64 {
    if (n > 20) return error.Overflow;
    if (n <= 1) return 1;
    return n * try factorial(n - 1);
}

// 测试块
test "add function" {
    try testing.expectEqual(@as(i32, 5), add(2, 3));
    try testing.expectEqual(@as(i32, 0), add(-1, 1));
    try testing.expectEqual(@as(i32, -5), add(-2, -3));
}

test "factorial function" {
    try testing.expectEqual(@as(u64, 1), try factorial(0));
    try testing.expectEqual(@as(u64, 1), try factorial(1));
    try testing.expectEqual(@as(u64, 120), try factorial(5));
    try testing.expectEqual(@as(u64, 2432902008176640000), try factorial(20));

    // 测试错误情况
    try testing.expectError(error.Overflow, factorial(21));
}
