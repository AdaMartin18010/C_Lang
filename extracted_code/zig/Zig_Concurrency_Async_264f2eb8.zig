//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 841
// Language: zig
// Block ID: 264f2eb8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// worker_pool.zig

const std = @import("std");
const Thread = std.Thread;
const Mutex = Thread.Mutex;
const Condition = Thread.Condition;
const Atomic = std.atomic.Atomic;

pub fn WorkerPool(comptime Task: type) type {
    const TaskFn = *const fn (Task) void;

    return struct {
        const Job = struct {
            task: Task,
            func: TaskFn,
        };

        allocator: std.mem.Allocator,
        workers: []Thread,
        queue: std.ArrayList(Job),
        mutex: Mutex,
        condition: Condition,
        shutdown: Atomic(bool),
        active_tasks: Atomic(usize),

        pub fn init(allocator: std.mem.Allocator, num_workers: usize) !@This() {
            var pool = @This(){
                .allocator = allocator,
                .workers = try allocator.alloc(Thread, num_workers),
                .queue = std.ArrayList(Job).init(allocator),
                .mutex = .{},
                .condition = .{},
                .shutdown = Atomic(bool).init(false),
                .active_tasks = Atomic(usize).init(0),
            };

            for (pool.workers) |*worker| {
                worker.* = try Thread.spawn(.{}, workerLoop, .{&pool});
            }

            return pool;
        }

        pub fn deinit(self: *@This()) void {
            self.shutdown.store(true, .Release);
            self.condition.broadcast();

            for (self.workers) |worker| {
                worker.join();
            }

            self.allocator.free(self.workers);
            self.queue.deinit();
        }

        pub fn submit(self: *@This(), task: Task, func: TaskFn) !void {
            self.mutex.lock();
            defer self.mutex.unlock();

            try self.queue.append(.{ .task = task, .func = func });
            self.condition.signal();
        }

        pub fn wait(self: *@This()) void {
            while (self.active_tasks.load(.Acquire) > 0 or self.queue.items.len > 0) {
                Thread.sleep(1 * std.time.ns_per_ms);
            }
        }

        fn workerLoop(pool: *@This()) void {
            while (!pool.shutdown.load(.Acquire)) {
                var job: ?Job = null;

                pool.mutex.lock();
                while (pool.queue.items.len == 0 and !pool.shutdown.load(.Acquire)) {
                    pool.condition.wait(&pool.mutex);
                }

                if (pool.queue.items.len > 0) {
                    job = pool.queue.pop();
                }
                pool.mutex.unlock();

                if (job) |j| {
                    _ = pool.active_tasks.fetchAdd(1, .Monotonic);
                    j.func(j.task);
                    _ = pool.active_tasks.fetchSub(1, .Monotonic);
                }
            }
        }
    };
}

// 使用示例
const ComputeTask = struct {
    id: usize,
    value: i32,
};

fn compute(task: ComputeTask) void {
    var result: i64 = 0;
    var i: i32 = 0;
    while (i < task.value) : (i += 1) {
        result += i;
    }
    std.log.info("Task {} result: {}", .{ task.id, result });
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var pool = try WorkerPool(ComputeTask).init(allocator, 4);
    defer pool.deinit();

    // 提交任务
    var i: usize = 0;
    while (i < 100) : (i += 1) {
        try pool.submit(.{ .id = i, .value = @intCast(i * 1000) }, compute);
    }

    // 等待完成
    pool.wait();
    std.log.info("All tasks completed", .{});
}
