//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 271
// Language: zig
// Block ID: d45d96ee
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// tests/integration_tests.zig

const std = @import("std");
const testing = std.testing;
const parser = @import("../src/parser.zig");

test "parse complete file" {
    const allocator = testing.allocator;

    // 读取测试文件
    const input = try std.fs.cwd().readFileAlloc(allocator, "tests/fixtures/input1.txt", 1024 * 1024);
    defer allocator.free(input);

    // 解析
    const result = try parser.parse(allocator, input);
    defer result.deinit();

    // 验证结果
    const expected = try std.fs.cwd().readFileAlloc(allocator, "tests/fixtures/expected1.txt", 1024 * 1024);
    defer allocator.free(expected);

    try testing.expectEqualStrings(expected, result.toString());
}

test "error handling" {
    const allocator = testing.allocator;

    const invalid_input = "invalid { json";

    // 应该返回错误
    const result = parser.parse(allocator, invalid_input);
    try testing.expectError(error.ParseError, result);
}
