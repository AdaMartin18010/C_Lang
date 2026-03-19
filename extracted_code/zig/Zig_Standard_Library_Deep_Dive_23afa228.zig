//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1366
// Language: zig
// Block ID: 23afa228
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 日志级别: err, warn, info, debug
const std = @import("std");

const log = std.log.scoped(.my_module);

pub fn main() void {
    // 使用 scoped 日志
    log.err("Error occurred: {}", .{42});
    log.warn("Warning: {}", .{"low memory"});
    log.info("Application started");
    log.debug("Debug info: x={}", .{100});

    // 断言
    const condition = true;
    std.debug.assert(condition);

    // 自定义断言消息
    if (!condition) {
        std.debug.panic("Condition failed!", .{});
    }

    // 打印栈跟踪
    const stack_trace = @import("builtin").stack_trace;
    std.debug.dumpStackTrace(stack_trace);

    // 内存转储
    const data = "ABCDEFGH";
    std.debug.print("Memory dump:\n", .{});
    for (data, 0..) |byte, i| {
        std.debug.print("[{d:0>4}] 0x{X:0>2} '{c}'\n", .{ i, byte, byte });
    }
}

// 自定义日志处理器
pub const std_options = struct {
    pub const log_level = .debug;

    pub fn logFn(
        comptime level: std.log.Level,
        comptime scope: @TypeOf(.EnumLiteral),
        comptime format: []const u8,
        args: anytype,
    ) void {
        const prefix = comptime level.asText() ++ " [" ++ @tagName(scope) ++ "] ";

        // 打印到 stderr
        std.debug.print(prefix ++ format ++ "\n", args);

        // 可以添加文件记录、网络发送等
    }
};
