//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 168
// Language: zig
// Block ID: 032b46ed
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const testing = std.testing;

fn processData(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    const output = try allocator.alloc(u8, input.len);
    for (input, 0..) |c, i| {
        output[i] = c + 1;  // Caesar cipher
    }
    return output;
}

test "processData allocates and processes" {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer testing.expect(!gpa.detectLeaks());  // 检查内存泄漏

    const allocator = gpa.allocator();
    const input = "hello";

    const result = try processData(allocator, input);
    defer allocator.free(result);

    try testing.expectEqualStrings("ifmmp", result);
}

// 使用测试分配器
test "with test allocator" {
    var test_allocator = testing.allocator;

    const data = try test_allocator.alloc(u8, 100);
    defer test_allocator.free(data);

    // 测试分配器会自动检测泄漏
}

// 固定缓冲区分配器测试
test "with fixed buffer allocator" {
    var buffer: [1024]u8 = undefined;
    var fba = std.heap.FixedBufferAllocator.init(&buffer);
    const allocator = fba.allocator();

    const data = try allocator.alloc(u8, 100);
    allocator.free(data);

    // 验证没有缓冲区溢出
    try testing.expect(fba.end_index <= buffer.len);
}
