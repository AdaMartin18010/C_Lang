//
// Auto-generated from: Zig\Zig_Standard_Library_Deep_Dive.md
// Line: 1327
// Language: zig
// Block ID: 5dcd76fd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn time_operations() !void {
    // 当前时间戳
    const timestamp_ms = std.time.milliTimestamp();
    const timestamp_us = std.time.microTimestamp();

    std.log.info("Timestamp (ms): {}", .{timestamp_ms});

    // 高精度计时
    var timer = try std.time.Timer.start();

    // 模拟工作
    std.time.sleep(10 * std.time.ns_per_ms);

    const elapsed_ns = timer.read();
    std.log.info("Elapsed: {} ns ({} ms)", .{ elapsed_ns, elapsed_ns / std.time.ns_per_ms });

    // 计时器重 lap
    const lap1 = timer.lap();
    std.time.sleep(5 * std.time.ns_per_ms);
    const lap2 = timer.lap();

    std.log.info("Lap1: {} ms, Lap2: {} ms", .{
        lap1 / std.time.ns_per_ms,
        lap2 / std.time.ns_per_ms
    });

    // 暂停 (忙等待)
    const start = std.time.milliTimestamp();
    while (std.time.milliTimestamp() - start < 100) {
        std.atomic.spinLoopHint();
    }
}
