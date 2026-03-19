//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 99
// Language: zig
// Block ID: b8c3e68d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 基本线程创建
    const thread = try std.Thread.spawn(.{}, worker, .{"Hello"});
    thread.join();

    // 带上下文参数的线程
    const data = Data{ .value = 42, .name = "worker1" };
    const thread2 = try std.Thread.spawn(.{}, workerWithStruct, .{data});
    thread2.join();

    // 多线程
    var threads: [10]std.Thread = undefined;
    for (&threads, 0..) |*t, i| {
        t.* = try std.Thread.spawn(.{}, numberedWorker, .{i});
    }
    for (threads) |t| {
        t.join();
    }
}

fn worker(msg: []const u8) void {
    std.log.info("Thread says: {s}", .{msg});
}

const Data = struct {
    value: i32,
    name: []const u8,
};

fn workerWithStruct(data: Data) void {
    std.log.info("Worker {s} has value {}", .{ data.name, data.value });
}

fn numberedWorker(id: usize) void {
    std.time.sleep(100 * std.time.ns_per_ms);
    std.log.info("Worker {} done", .{id});
}
