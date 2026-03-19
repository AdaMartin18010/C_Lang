//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\02_SQLite_BTree_Migration.md
// Line: 599
// Language: zig
// Block ID: 4851f792
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// mem.zig - SQLite 风格的内存分配器
const std = @import("std");

pub const SQLiteAllocator = struct {
    allocator: std.mem.Allocator,
    stats: Stats,

    const Stats = struct {
        current: usize,
        highwater: usize,
        total: usize,
        count: usize,
    };

    pub fn init(allocator: std.mem.Allocator) SQLiteAllocator {
        return .{
            .allocator = allocator,
            .stats = .{
                .current = 0,
                .highwater = 0,
                .total = 0,
                .count = 0,
            },
        };
    }

    /// malloc 风格分配
    pub fn malloc(self: *SQLiteAllocator, size: usize) ?[*]u8 {
        const ptr = self.allocator.alloc(u8, size) catch return null;

        self.stats.current += size;
        self.stats.total += size;
        self.stats.count += 1;
        if (self.stats.current > self.stats.highwater) {
            self.stats.highwater = self.stats.current;
        }

        return ptr.ptr;
    }

    /// free 风格释放
    pub fn free(self: *SQLiteAllocator, ptr: ?[*]u8, size: usize) void {
        if (ptr) |p| {
            self.allocator.free(p[0..size]);
            self.stats.current -= size;
        }
    }

    /// realloc 风格重新分配
    pub fn realloc(self: *SQLiteAllocator, ptr: ?[*]u8, old_size: usize, new_size: usize) ?[*]u8 {
        if (ptr == null) return self.malloc(new_size);
        if (new_size == 0) {
            self.free(ptr, old_size);
            return null;
        }

        const new_ptr = self.malloc(new_size) orelse return null;
        @memcpy(new_ptr[0..@min(old_size, new_size)], ptr.?[0..@min(old_size, new_size)]);
        self.free(ptr, old_size);

        return new_ptr;
    }

    pub fn getStats(self: *const SQLiteAllocator) Stats {
        return self.stats;
    }
};
