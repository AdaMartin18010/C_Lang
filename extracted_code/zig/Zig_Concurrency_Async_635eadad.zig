//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 347
// Language: zig
// Block ID: 635eadad
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const Atomic = std.atomic.Atomic;

// 原子变量
var atomic_counter = Atomic(usize).init(0);
var atomic_flag = Atomic(bool).init(false);

// 原子递增
fn atomicIncrement() void {
    _ = atomic_counter.fetchAdd(1, .Monotonic);
}

// CAS (比较并交换)
fn atomicCAS() void {
    var expected: usize = 0;
    while (!atomic_counter.compareAndSwap(
        expected,
        expected + 1,
        .Acquire,
        .Monotonic,
    )) |actual| {
        expected = actual;
    }
}

// 自旋锁
const SpinLock = struct {
    locked: Atomic(bool),

    pub fn init() SpinLock {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *SpinLock) void {
        while (self.locked.swap(true, .Acquire)) {
            std.atomic.spinLoopHint();
        }
    }

    pub fn unlock(self: *SpinLock) void {
        self.locked.store(false, .Release);
    }
};
