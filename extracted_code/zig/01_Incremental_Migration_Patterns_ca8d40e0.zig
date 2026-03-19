//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
// Line: 2170
// Language: zig
// Block ID: ca8d40e0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 同时测试 C 和 Zig 实现

const std = @import("std");

/// 双重测试运行器
pub fn DualTestRunner(comptime CImpl: type, comptime ZigImpl: type) type {
    return struct {
        /// 对比两个实现的输出
        pub fn compareOutputs(
            test_name: []const u8,
            input: []const u8,
        ) !void {
            std.debug.print("Testing: {s}\n", .{test_name});

            // 运行 C 实现
            const c_result = CImpl.process(input);
            defer CImpl.freeResult(c_result);

            // 运行 Zig 实现
            const zig_result = ZigImpl.process(input) catch |e| {
                std.debug.print("Zig implementation failed: {}\n", .{e});
                return e;
            };
            defer ZigImpl.freeResult(zig_result);

            // 对比结果
            if (!std.mem.eql(u8, c_result.output, zig_result.output)) {
                std.debug.print("Output mismatch!\n", .{});
                std.debug.print("C:   {s}\n", .{c_result.output});
                std.debug.print("Zig: {s}\n", .{zig_result.output});
                return error.OutputMismatch;
            }

            if (c_result.status != zig_result.status) {
                std.debug.print("Status mismatch: C={}, Zig={}\n", .{
                    c_result.status,
                    zig_result.status,
                });
                return error.StatusMismatch;
            }

            std.debug.print("✓ {s} passed\n", .{test_name});
        }
    };
}

// 使用示例
const ParserTest = DualTestRunner(CParser, ZigParser);

test "parser equivalence" {
    try ParserTest.compareOutputs(
        "simple config",
        "key = value\n",
    );

    try ParserTest.compareOutputs(
        "nested section",
        "[section]\nkey = value\n",
    );

    try ParserTest.compareOutputs(
        "empty input",
        "",
    );
}
