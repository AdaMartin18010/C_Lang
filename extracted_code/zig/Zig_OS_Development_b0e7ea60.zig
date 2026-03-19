//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 972
// Language: zig
// Block ID: b0e7ea60
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// spinlock.zig - 自旋锁实现
const std = @import("std");

pub const SpinLock = struct {
    locked: std.atomic.Value(u32),

    pub fn init() SpinLock {
        return .{ .locked = std.atomic.Value(u32).init(0) };
    }

    pub fn acquire(self: *SpinLock) void {
        while (self.locked.compareAndSwap(0, 1, .acquire, .monotonic) != null) {
            // 自旋等待
            while (self.locked.load(.relaxed) != 0) {
                asm volatile ("pause");
            }
        }
    }

    pub fn release(self: *SpinLock) void {
        self.locked.store(0, .release);
    }
};
