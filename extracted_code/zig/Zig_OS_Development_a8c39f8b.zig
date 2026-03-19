//
// Auto-generated from: Zig\Zig_OS_Development.md
// Line: 1000
// Language: zig
// Block ID: a8c39f8b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// semaphore.zig - 信号量实现
const std = @import("std");

pub const Semaphore = struct {
    count: std.atomic.Value(i32),
    wait_queue: std.ArrayList(*Thread),

    pub fn init(initial: i32) Semaphore {
        return .{
            .count = std.atomic.Value(i32).init(initial),
            .wait_queue = undefined,
        };
    }

    pub fn wait(self: *Semaphore) void {
        while (true) {
            const current = self.count.load(.acquire);
            if (current > 0) {
                if (self.count.compareAndSwap(
                    current, current - 1, .acquire, .monotonic
                ) == null) {
                    return;
                }
            } else {
                // 阻塞并加入等待队列
                break;
            }
        }
    }

    pub fn signal(self: *Semaphore) void {
        _ = self.count.fetchAdd(1, .release);
        // 唤醒等待队列中的线程
    }
};
