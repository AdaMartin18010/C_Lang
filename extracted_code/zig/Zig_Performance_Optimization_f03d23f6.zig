//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 834
// Language: zig
// Block ID: f03d23f6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 无锁环形缓冲区
pub const LockFreeRingBuffer = struct {
    buffer: []u8,
    head: Atomic(usize),
    tail: Atomic(usize),

    pub fn init(allocator: std.mem.Allocator, size: usize) !LockFreeRingBuffer {
        return .{
            .buffer = try allocator.alloc(u8, size),
            .head = Atomic(usize).init(0),
            .tail = Atomic(usize).init(0),
        };
    }

    pub fn write(self: *LockFreeRingBuffer, data: []const u8) bool {
        const head = self.head.load(.Acquire);
        const tail = self.tail.load(.Monotonic);
        const size = self.buffer.len;

        const available = if (head >= tail)
            size - (head - tail)
        else
            tail - head - 1;

        if (data.len > available - 1) return false;

        // 写入数据
        for (data, 0..) |byte, i| {
            self.buffer[(head + i) % size] = byte;
        }

        self.head.store((head + data.len) % size, .Release);
        return true;
    }

    pub fn read(self: *LockFreeRingBuffer, out: []u8) usize {
        const head = self.head.load(.Monotonic);
        const tail = self.tail.load(.Acquire);
        const size = self.buffer.len;

        const available = if (head >= tail)
            head - tail
        else
            size - tail + head;

        const to_read = @min(out.len, available);

        for (0..to_read) |i| {
            out[i] = self.buffer[(tail + i) % size];
        }

        self.tail.store((tail + to_read) % size, .Release);
        return to_read;
    }
};

const Atomic = std.atomic.Atomic;
const std = @import("std");
