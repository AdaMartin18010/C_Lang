//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1538
// Language: zig
// Block ID: ae34828b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// Zig 的异步 I/O (需要事件循环)
const EventLoop = struct {
    // 简化示例
};

// 基本线程
fn basic_threads() !void {
    const allocator = std.heap.page_allocator;

    var handles: [10]std.Thread = undefined;

    for (&handles, 0..) |*handle, i| {
        handle.* = try std.Thread.spawn(.{}, worker, .{i});
    }

    for (handles) |handle| {
        handle.join();
    }
}

fn worker(id: usize) void {
    std.time.sleep(100 * std.time.ns_per_ms);
    std.debug.print("Worker {} done\n", .{id});
}

// Mutex 和 Condition Variable
fn thread_synchronization() !void {
    const Context = struct {
        mutex: std.Thread.Mutex = .{},
        cond: std.Thread.Condition = .{},
        ready: bool = false,
        data: i32 = 0,
    };

    var ctx: Context = .{};

    const producer = struct {
        fn run(c: *Context) void {
            c.mutex.lock();
            defer c.mutex.unlock();

            c.data = 42;
            c.ready = true;
            c.cond.signal();
        }
    }.run;

    const consumer = struct {
        fn run(c: *Context) void {
            c.mutex.lock();
            defer c.mutex.unlock();

            while (!c.ready) {
                c.cond.wait(&c.mutex);
            }
            std.debug.print("Got data: {}\n", .{c.data});
        }
    }.run;

    const t1 = try std.Thread.spawn(.{}, producer, .{&ctx});
    const t2 = try std.Thread.spawn(.{}, consumer, .{&ctx});

    t1.join();
    t2.join();
}

// 原子操作
fn atomic_operations() void {
    var counter: std.atomic.Value(u32) = std.atomic.Value(u32).init(0);

    // fetch_add 返回旧值
    const old = counter.fetchAdd(1, .seq_cst);
    std.debug.print("Old: {}, New: {}\n", .{ old, counter.load(.seq_cst) });

    // CAS (Compare and Swap)
    while (true) {
        const current = counter.load(.seq_cst);
        if (counter.cmpxchgStrong(current, current + 1, .seq_cst, .seq_cst)) |_| {
            break;
        }
    }
}

pub fn main() !void {
    try basic_threads();
    try thread_synchronization();
    atomic_operations();
}
