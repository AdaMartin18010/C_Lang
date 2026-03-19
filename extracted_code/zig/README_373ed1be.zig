//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\README.md
// Line: 17
// Language: zig
// Block ID: 373ed1be
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 原始 C 的 sds.c (Simple Dynamic Strings)
// 迁移为 Zig 的 sds.zig

const std = @import("std");
const Allocator = std.mem.Allocator;

/// SDS (Simple Dynamic String) 结构
/// 内存布局: [len:4][alloc:4][flags:1][buf:N+1]
pub const SDS = struct {
    allocator: Allocator,
    buf: []u8,

    const Self = @This();
    const HDR_SIZE = 9; // 4(len) + 4(alloc) + 1(flags)

    /// 创建新的 SDS
    pub fn new(allocator: Allocator, init_str: ?[]const u8) !Self {
        const len = if (init_str) |s| s.len else 0;
        const alloc_size = HDR_SIZE + len + 1;

        const mem = try allocator.alloc(u8, alloc_size);

        // 写入头部
        std.mem.writeInt(u32, mem[0..4], @intCast(len), .little);
        std.mem.writeInt(u32, mem[4..8], @intCast(alloc_size - HDR_SIZE), .little);
        mem[8] = 0;

        if (init_str) |s| {
            @memcpy(mem[HDR_SIZE..][0..len], s);
        }
        mem[HDR_SIZE + len] = 0;

        return Self{
            .allocator = allocator,
            .buf = mem[HDR_SIZE..][0..len],
        };
    }

    pub fn len(self: Self) usize {
        return self.buf.len;
    }

    /// 追加字符串
    pub fn append(self: *Self, str: []const u8) !void {
        if (str.len == 0) return;
        const required = self.buf.len + str.len;

        if (required > self.buf.len + self.avail()) {
            const new_alloc = required + required / 2;
            try self.grow(new_alloc);
        }

        @memcpy(self.buf.ptr + self.buf.len, str);
        self.buf.len += str.len;
        self.buf.ptr[self.buf.len] = 0;
    }

    fn avail(self: Self) usize {
        const full_slice = self.getFullSlice();
        const alloc_size = std.mem.readInt(u32, full_slice[4..8], .little);
        return alloc_size - self.buf.len - 1;
    }

    fn getFullSlice(self: Self) []u8 {
        const ptr = @intFromPtr(self.buf.ptr) - HDR_SIZE;
        const len = std.mem.readInt(u32, @as([*]u8, @ptrFromInt(ptr))[0..4], .little);
        return @as([*]u8, @ptrFromInt(ptr))[0..HDR_SIZE + len + 1];
    }

    fn grow(self: *Self, new_size: usize) !void {
        const full = self.getFullSlice();
        const new_full = try self.allocator.realloc(full, HDR_SIZE + new_size + 1);
        std.mem.writeInt(u32, new_full[4..8], @intCast(new_size), .little);
        self.buf.ptr = new_full.ptr + HDR_SIZE;
    }

    pub fn free(self: *Self) void {
        const full = self.getFullSlice();
        self.allocator.free(full);
        self.buf = &[_]u8{};
    }
};
