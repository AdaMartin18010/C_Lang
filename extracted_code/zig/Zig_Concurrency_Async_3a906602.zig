//
// Auto-generated from: Zig\Zig_Concurrency_Async.md
// Line: 638
// Language: zig
// Block ID: 3a906602
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 有缓冲通道
const std = @import("std");
const Mutex = std.Thread.Mutex;
const Condition = std.Thread.Condition;

pub fn Channel(comptime T: type) type {
    return struct {
        allocator: std.mem.Allocator,
        buffer: []T,
        capacity: usize,
        head: usize,
        tail: usize,
        count: usize,
        closed: bool,
        mutex: Mutex,
        not_full: Condition,
        not_empty: Condition,

        pub fn init(allocator: std.mem.Allocator, capacity: usize) !@This() {
            const buffer = try allocator.alloc(T, capacity);

            return .{
                .allocator = allocator,
                .buffer = buffer,
                .capacity = capacity,
                .head = 0,
                .tail = 0,
                .count = 0,
                .closed = false,
                .mutex = .{},
                .not_full = .{},
                .not_empty = .{},
            };
        }

        pub fn deinit(self: *@This()) void {
            self.allocator.free(self.buffer);
        }

        pub fn send(self: *@This(), value: T) !void {
            self.mutex.lock();
            defer self.mutex.unlock();

            while (self.count == self.capacity and !self.closed) {
                self.not_full.wait(&self.mutex);
            }

            if (self.closed) return error.Closed;

            self.buffer[self.tail] = value;
            self.tail = (self.tail + 1) % self.capacity;
            self.count += 1;

            self.not_empty.signal();
        }

        pub fn receive(self: *@This()) !T {
            self.mutex.lock();
            defer self.mutex.unlock();

            while (self.count == 0 and !self.closed) {
                self.not_empty.wait(&self.mutex);
            }

            if (self.count == 0 and self.closed) return error.Closed;

            const value = self.buffer[self.head];
            self.head = (self.head + 1) % self.capacity;
            self.count -= 1;

            self.not_full.signal();
            return value;
        }

        pub fn close(self: *@This()) void {
            self.mutex.lock();
            defer self.mutex.unlock();

            self.closed = true;
            self.not_full.broadcast();
            self.not_empty.broadcast();
        }
    };
}
