//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 1249
// Language: zig
// Block ID: b57dd76c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig Spinlock 实现
const std = @import("std");
const Atomic = std.atomic.Value;

pub const Spinlock = struct {
    locked: Atomic(bool),

    pub const Self = @This();

    pub fn init() Self {
        return .{ .locked = Atomic(bool).init(false) };
    }

    pub fn lock(self: *Self) void {
        // 测试并测试-设置
        while (true) {
            // 首先宽松检查
            if (!self.locked.load(.Monotonic)) {
                // 尝试获取锁
                const acquired = self.locked.tryCmpxchg(
                    false,
                    true,
                    .Acquire,    // 成功时获取
                    .Monotonic   // 失败时宽松
                );
                if (acquired == null) {
                    return;  // 成功获取锁
                }
            }
            // 自旋等待 - x86 上使用 pause 指令
            std.atomic.spinLoopHint();
        }
    }

    pub fn tryLock(self: *Self) bool {
        const result = self.locked.tryCmpxchg(
            false,
            true,
            .Acquire,
            .Monotonic
        );
        return result == null;
    }

    pub fn unlock(self: *Self) void {
        // Release: 确保锁内所有写入在解锁后可见
        self.locked.store(false, .Release);
    }

    // Zig 惯用法: 提供 LockGuard 模式
    pub fn acquire(self: *Self) LockGuard {
        self.lock();
        return LockGuard{ .lock = self };
    }
};

pub const LockGuard = struct {
    lock: *Spinlock,

    pub fn release(self: LockGuard) void {
        self.lock.unlock();
    }
};

// 使用示例
var g_lock = Spinlock.init();
var shared_data: i32 = 0;

fn worker() void {
    var i: i32 = 0;
    while (i < 1000) : (i += 1) {
        const guard = g_lock.acquire();
        shared_data += 1;  // 临界区
        guard.release();
    }
}

// 或使用 defer 模式
fn workerWithDefer() void {
    var i: i32 = 0;
    while (i < 1000) : (i += 1) {
        g_lock.lock();
        defer g_lock.unlock();
        shared_data += 1;  // 临界区
    }
}
