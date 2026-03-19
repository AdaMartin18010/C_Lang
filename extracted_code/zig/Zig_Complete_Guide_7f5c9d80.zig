//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1635
// Language: zig
// Block ID: 7f5c9d80
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 普通单元测试
test "basic arithmetic" {
    try std.testing.expectEqual(2 + 2, 4);
    try std.testing.expect(2 + 2 == 4);
}

test "array operations" {
    var arr = [_]i32{ 1, 2, 3 };
    try std.testing.expectEqual(arr.len, 3);
    try std.testing.expectEqual(arr[0], 1);
}

// 使用分配的测试
test "allocator test" {
    const allocator = std.testing.allocator;

    const buf = try allocator.alloc(u8, 100);
    defer allocator.free(buf);

    @memset(buf, 0xAA);
    try std.testing.expectEqual(buf[50], 0xAA);
}

// 失败测试
test "expected failure" {
    const result = failing_function();
    try std.testing.expectError(error.SomeError, result);
}

fn failing_function() !void {
    return error.SomeError;
}

// 模糊测试 (Zig 内置支持)
test "fuzz example" {
    const input_bytes = std.testing.fuzzInput(.{});

    // 使用输入进行模糊测试
    if (input_bytes.len > 10) return;

    // 测试解析器是否会崩溃
    _ = parseSomething(input_bytes) catch return;
}

fn parseSomething(data: []const u8) !i32 {
    if (data.len < 4) return error.TooShort;
    if (data[0] != 0xAA) return error.InvalidHeader;
    return 42;
}

// 基准测试
var bench_result: i64 = 0;

test "benchmark" {
    const iterations: usize = 1_000_000;

    var timer = try std.time.Timer.start();

    var sum: i64 = 0;
    for (0..iterations) |i| {
        sum += @intCast(i);
    }
    bench_result = sum;

    const elapsed_ns = timer.read();
    const elapsed_ms = elapsed_ns / std.time.ns_per_ms;

    std.debug.print("Benchmark: {} iterations in {} ms\n", .{ iterations, elapsed_ms });
}
