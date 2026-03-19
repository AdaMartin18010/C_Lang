//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\01_Redis_Data_Structure_Migration.md
// Line: 665
// Language: zig
// Block ID: 759f9050
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// zmalloc.zig - 内存池实现
const std = @import("std");
const Allocator = std.mem.Allocator;

pub const ZmallocAllocator = struct {
    allocator: Allocator,
    used_memory: std.atomic.Value(usize),

    pub fn init(allocator: Allocator) ZmallocAllocator {
        return .{
            .allocator = allocator,
            .used_memory = std.atomic.Value(usize).init(0),
        };
    }

    pub fn alloc(self: *ZmallocAllocator, len: usize) ![]u8 {
        const result = try self.allocator.alloc(u8, len);
        _ = self.used_memory.fetchAdd(len, .Monotonic);
        return result;
    }

    pub fn free(self: *ZmallocAllocator, buf: []u8) void {
        _ = self.used_memory.fetchSub(buf.len, .Monotonic);
        self.allocator.free(buf);
    }

    pub fn usedMemory(self: *ZmallocAllocator) usize {
        return self.used_memory.load(.Monotonic);
    }
};
