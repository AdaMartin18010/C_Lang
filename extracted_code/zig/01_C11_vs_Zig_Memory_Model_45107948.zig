//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 1624
// Language: zig
// Block ID: 45107948
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 生产者-消费者 (有界缓冲区)
const std = @import("std");
const Atomic = std.atomic.Value;

pub fn RingBuffer(comptime size: usize) type {
    return struct {
        const Self = @This();

        buffer: [size]u8,
        write_idx: Atomic(usize),
        read_idx: Atomic(usize),

        pub fn init() Self {
            return .{
                .buffer = undefined,
                .write_idx = Atomic(usize).init(0),
                .read_idx = Atomic(usize).init(0),
            };
        }

        // 返回可写入的字节数
        pub fn writeAvail(self: *const Self) usize {
            const write_idx = self.write_idx.load(.Monotonic);
            const read_idx = self.read_idx.load(.Acquire);
            return (size - 1 + read_idx - write_idx) % size;
        }

        // 返回可读取的字节数
        pub fn readAvail(self: *const Self) usize {
            const write_idx = self.write_idx.load(.Acquire);
            const read_idx = self.read_idx.load(.Monotonic);
            return (size + write_idx - read_idx) % size;
        }

        pub fn write(self: *Self, data: []const u8) bool {
            if (self.writeAvail() < data.len) {
                return false;  // 空间不足
            }

            const write_idx = self.write_idx.load(.Monotonic);

            for (data, 0..) |byte, i| {
                self.buffer[(write_idx + i) % size] = byte;
            }

            // Release: 确保数据写入在索引更新前可见
            self.write_idx.store((write_idx + data.len) % size, .Release);
            return true;
        }

        pub fn read(self: *Self, data: []u8) bool {
            if (self.readAvail() < data.len) {
                return false;  // 数据不足
            }

            const read_idx = self.read_idx.load(.Monotonic);

            for (data, 0..) |*byte, i| {
                byte.* = self.buffer[(read_idx + i) % size];
            }

            // Release: 确保读取完成在索引更新前可见
            self.read_idx.store((read_idx + data.len) % size, .Release);
            return true;
        }

        // 使用切片 API 的惯用版本
        pub fn writeSlice(self: *Self, data: []const u8) ?[]const u8 {
            const avail = self.writeAvail();
            const to_write = @min(avail, data.len);
            if (to_write == 0) return null;

            const write_idx = self.write_idx.load(.Monotonic);

            for (0..to_write) |i| {
                self.buffer[(write_idx + i) % size] = data[i];
            }

            self.write_idx.store((write_idx + to_write) % size, .Release);
            return data[0..to_write];
        }
    };
}
