//
// Auto-generated from: Zig\Zig_Testing_Debugging.md
// Line: 473
// Language: zig
// Block ID: 134efb16
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const Timer = struct {
    name: []const u8,
    start: i128,

    pub fn begin(name: []const u8) Timer {
        return .{
            .name = name,
            .start = std.time.nanoTimestamp(),
        };
    }

    pub fn end(self: Timer) void {
        const elapsed = std.time.nanoTimestamp() - self.start;
        const ms = @as(f64, @floatFromInt(elapsed)) / std.time.ns_per_ms;
        std.debug.print("[{}] took {d:.3} ms\n", .{ self.name, ms });
    }
};

// 使用
fn measuredFunction() void {
    const timer = Timer.begin("operation");
    defer timer.end();

    // 执行操作
    std.time.sleep(10 * std.time.ns_per_ms);
}
