//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 406
// Language: zig
// Block ID: dd3dbac4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 设置日志级别
pub const std_options = .{
    .log_level = .debug,  // .err, .warn, .info, .debug
};

fn doSomething() void {
    std.log.err("This is an error: {}", .{42});
    std.log.warn("This is a warning");
    std.log.info("This is info");
    std.log.debug("This is debug: {s}", .{"details"});
}

// 作用域日志
const my_log = std.log.scoped(.my_module);

fn scopedLogging() void {
    my_log.info("Module-specific log");
}
