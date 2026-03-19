//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 337
// Language: zig
// Block ID: 8de436b8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

fn debugMe(value: i32) i32 {
    // 编译时断点
    @breakpoint();

    // 条件断点
    if (value < 0) {
        @breakpoint();
    }

    // 使用日志
    std.log.debug("debugMe called with value: {}", .{value});

    return value * 2;
}

// 编译时断言
fn compileTimeCheck(comptime T: type) void {
    comptime {
        if (@sizeOf(T) > 1024) {
            @compileError("Type too large: " ++ @typeName(T));
        }
    }
}

// 运行时断言
fn runtimeCheck(value: i32) i32 {
    std.debug.assert(value >= 0);  // debug 模式检查

    if (value < 0) {
        @panic("Value must be non-negative");  // 总是检查
    }

    return value;
}
