//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 754
// Language: zig
// Block ID: 2835b777
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 原子操作语义示例
const std = @import("std");

// 1. 基本原子操作
fn atomicBasics() void {
    var counter = std.atomic.Value(i32).init(0);

    // 原子加
    const prev = counter.fetchAdd(1, .monotonic);
    _ = prev;

    // 原子交换
    const old = counter.swap(10, .monotonic);
    _ = old;

    // 比较并交换 (CAS)
    const exchanged = counter.cmpxchgStrong(10, 20, .acq_rel, .monotonic);
    _ = exchanged;
}

// 2. 自旋锁实现 (使用原子操作)
const AtomicSpinLock = struct {
    locked: std.atomic.Value(bool),

    pub fn init() AtomicSpinLock {
        return .{ .locked = std.atomic.Value(bool).init(false) };
    }

    pub fn acquire(self: *AtomicSpinLock) void {
        // 使用 acquire 语义获取锁
        while (self.locked.swap(true, .acquire)) {
            // 自旋等待
            while (self.locked.load(.relaxed)) {
                std.atomic.spinLoopHint();
            }
        }
    }

    pub fn release(self: *AtomicSpinLock) void {
        // 使用 release 语义释放锁
        self.locked.store(false, .release);
    }
};

// 3. 无锁队列 (Michael-Scott 队列)
fn LockFreeQueue(comptime T: type) type {
    const Node = struct {
        data: T,
        next: std.atomic.Value(?*Node),
    };

    return struct {
        head: std.atomic.Value(*Node),
        tail: std.atomic.Value(*Node),

        pub fn push(self: @This(), value: T, allocator: std.mem.Allocator) !void {
            const node = try allocator.create(Node);
            node.* = .{
                .data = value,
                .next = std.atomic.Value(?*Node).init(null),
            };

            // 使用 release 保证 node 初始化对其他线程可见
            const prev_tail = self.tail.swap(node, .acq_rel);
            prev_tail.next.store(node, .release);
        }

        pub fn pop(self: @This()) ?T {
            // 使用 acquire 保证看到完整的 node
            const head_node = self.head.load(.acquire);
            const next_node = head_node.next.load(.acquire);

            if (next_node) |next| {
                self.head.store(next, .release);
                return next.data;
            }
            return null;
        }
    };
}

// 4. 内存屏障显式控制
fn memoryBarriers() void {
    var x: i32 = 0;
    var y: i32 = 0;

    // 写屏障: 保证 barrier 之前的写不会被重排序到之后
    x = 1;
    std.atomic.fence(.release);
    y = 1;

    // 读屏障: 保证 barrier 之后的读不会被重排序到之前
    const y_val = y;
    std.atomic.fence(.acquire);
    const x_val = x;

    _ = x_val;
    _ = y_val;
}
