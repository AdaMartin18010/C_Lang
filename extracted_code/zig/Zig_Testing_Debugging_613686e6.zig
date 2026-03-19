//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 378
// Language: zig
// Block ID: 613686e6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 获取堆栈跟踪
fn printStackTrace() void {
    const trace = @errorReturnTrace();
    if (trace) |t| {
        std.debug.print("Stack trace:\n", .{});
        for (t.instruction_addresses) |addr| {
            std.debug.print("  {x}\n", .{addr});
        }
    }
}

// 自定义 panic 处理器
pub fn panic(msg: []const u8, error_return_trace: ?*std.builtin.StackTrace, ret_addr: ?usize) noreturn {
    std.debug.print("PANIC: {s}\n", .{msg});

    if (error_return_trace) |trace| {
        std.debug.dumpStackTrace(trace.*);
    }

    _ = ret_addr;

    std.process.exit(1);
}
