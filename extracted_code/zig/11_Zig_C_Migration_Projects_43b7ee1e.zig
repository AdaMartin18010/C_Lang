//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 386
// Language: zig
// Block ID: 43b7ee1e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// test_runner.zig - 替换C的测试框架
const std = @import("std");

// 导入被测试的C代码
extern fn legacy_function(input: i32) i32;

test "legacy function works" {
    try std.testing.expectEqual(legacy_function(5), 10);
}

test "edge cases" {
    try std.testing.expectEqual(legacy_function(0), 0);
    try std.testing.expectEqual(legacy_function(-5), -10);
}
