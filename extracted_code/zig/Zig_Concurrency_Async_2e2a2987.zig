//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 174
// Language: zig
// Block ID: 2e2a2987
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用共享内存传递结果
const Result = struct {
    value: i32,
    completed: std.atomic.Atomic(bool),
};

fn workerWithResult(result: *Result) void {
    // 执行计算
    const computation = expensiveComputation();

    @atomicStore(i32, &result.value, computation, .Release);
    @atomicStore(bool, &result.completed, true, .Release);
}

// 使用通道 (Channel)
const Channel = @import("std").sync.Channel;

fn workerWithChannel(ch: *Channel(i32)) !void {
    const result = compute();
    try ch.send(result);
}

pub fn main() !void {
    var ch = Channel(i32).init(allocator, 1);
    defer ch.deinit();

    const thread = try std.Thread.spawn(.{}, workerWithChannel, .{&ch});

    const result = try ch.receive();
    std.log.info("Got result: {}", .{result});

    thread.join();
}
