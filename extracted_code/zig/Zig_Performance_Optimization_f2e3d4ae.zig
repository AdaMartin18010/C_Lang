//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 81
// Language: zig
// Block ID: f2e3d4ae
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const time = std.time;

// 高精度计时器
pub const Timer = struct {
    start_time: i128,
    name: []const u8,

    pub fn start(name: []const u8) Timer {
        return .{
            .start_time = time.nanoTimestamp(),
            .name = name,
        };
    }

    pub fn stop(self: Timer) void {
        const elapsed = time.nanoTimestamp() - self.start_time;
        const ms = @as(f64, @floatFromInt(elapsed)) / time.ns_per_ms;
        std.debug.print("[{}] {d:.3} ms\n", .{ self.name, ms });
    }
};

// 使用示例
pub fn benchmarkExample() void {
    const timer = Timer.start("computation");
    defer timer.stop();

    // 被测试的代码
    var sum: u64 = 0;
    for (0..1_000_000) |i| {
        sum += i;
    }
    std.mem.doNotOptimizeAway(sum);
}

// 多次运行的基准测试
pub fn benchmarkMultiple(
    comptime name: []const u8,
    comptime iterations: usize,
    func: fn () void,
) void {
    var total_time: i128 = 0;
    var min_time: i128 = std.math.maxInt(i128);
    var max_time: i128 = 0;

    var i: usize = 0;
    while (i < iterations) : (i += 1) {
        const start = time.nanoTimestamp();
        func();
        const elapsed = time.nanoTimestamp() - start;

        total_time += elapsed;
        min_time = @min(min_time, elapsed);
        max_time = @max(max_time, elapsed);
    }

    const avg = @as(f64, @floatFromInt(total_time)) / @as(f64, @floatFromInt(iterations)) / time.ns_per_ms;
    const min_ms = @as(f64, @floatFromInt(min_time)) / time.ns_per_ms;
    const max_ms = @as(f64, @floatFromInt(max_time)) / time.ns_per_ms;

    std.debug.print(
        \\[{s}] {} iterations
        \\  avg: {d:.3} ms
        \\  min: {d:.3} ms
        \\  max: {d:.3} ms
        \\
    , .{ name, iterations, avg, min_ms, max_ms });
}
