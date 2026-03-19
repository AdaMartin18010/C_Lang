//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\01_Redis_Data_Structure_Migration.md
// Line: 578
// Language: zig
// Block ID: 2be0a33e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// dict.zig - Redis 字典 Zig 实现
const std = @import("std");
const Allocator = std.mem.Allocator;

pub fn Dict(comptime K: type, comptime V: type, comptime hashFn: fn (K) u64) type {
    return struct {
        const Self = @This();

        const Entry = struct {
            key: K,
            value: V,
            next: ?*Entry,
        };

        const Table = struct {
            table: []*?Entry,
            size: usize,
            sizemask: usize,
            used: usize,
        };

        ht: [2]Table,
        rehashidx: isize,  // -1 表示不在 rehash
        allocator: Allocator,

        pub fn init(allocator: Allocator) Self {
            return Self{
                .ht = undefined,
                .rehashidx = -1,
                .allocator = allocator,
            };
        }

        pub fn expand(self: *Self, size: usize) !void {
            const real_size = nextPowerOfTwo(size);
            if (self.isRehashing() or self.ht[0].used > size) return;

            self.ht[1] = Table{
                .table = try self.allocator.alloc(*?Entry, real_size),
                .size = real_size,
                .sizemask = real_size - 1,
                .used = 0,
            };

            @memset(self.ht[1].table, null);
            self.rehashidx = 0;
        }

        fn nextPowerOfTwo(n: usize) usize {
            var size: usize = 1;
            while (size < n) size <<= 1;
            return size;
        }

        fn isRehashing(self: Self) bool {
            return self.rehashidx != -1;
        }

        pub fn add(self: *Self, key: K, value: V) !void {
            if (self.isRehashing()) try self.rehashStep();

            const entry = try self.allocator.create(Entry);
            entry.* = .{
                .key = key,
                .value = value,
                .next = null,
            };

            const idx = hashFn(key) & self.ht[0].sizemask;
            entry.next = self.ht[0].table[idx];
            self.ht[0].table[idx] = entry;
            self.ht[0].used += 1;
        }

        fn rehashStep(self: *Self) !void {
            // 渐进式 rehash 实现...
            _ = self;
        }
    };
}
