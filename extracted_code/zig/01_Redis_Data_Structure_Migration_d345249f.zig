//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\01_Redis_Data_Structure_Migration.md
// Line: 157
// Language: zig
// Block ID: d345249f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// sds.zig - Zig 实现，保持 C ABI 兼容
const std = @import("std");
const Allocator = std.mem.Allocator;

// C ABI 兼容的导出类型
pub const Sds = [*c]u8;

// Zig 内部结构 - 使用 packed struct 确保布局
const SdsHdr8 = packed struct {
    len: u8,
    alloc: u8,
    flags: u8,
    // buf 跟随在结构体后
};

const SdsHdr16 = packed struct {
    len: u16,
    alloc: u16,
    flags: u8,
};

const SdsHdr32 = packed struct {
    len: u32,
    alloc: u32,
    flags: u8,
};

const SdsHdr64 = packed struct {
    len: u64,
    alloc: u64,
    flags: u8,
};

const SDS_TYPE_5: u8 = 0;
const SDS_TYPE_8: u8 = 1;
const SDS_TYPE_16: u8 = 2;
const SDS_TYPE_32: u8 = 3;
const SDS_TYPE_64: u8 = 4;
const SDS_TYPE_MASK: u8 = 7;

// 全局分配器 (可配置)
var g_allocator: ?Allocator = null;

pub fn setAllocator(allocator: Allocator) void {
    g_allocator = allocator;
}

fn getAllocator() Allocator {
    return g_allocator orelse std.heap.c_allocator;
}

// 辅助函数: 选择合适的头部类型
fn sdsReqType(len: usize) u8 {
    if (len < 32) return SDS_TYPE_5;
    if (len < 256) return SDS_TYPE_8;
    if (len < 65536) return SDS_TYPE_16;
    if (len < 4294967296) return SDS_TYPE_32;
    return SDS_TYPE_64;
}

fn hdrSize(flags: u8) usize {
    return switch (flags & SDS_TYPE_MASK) {
        SDS_TYPE_5 => 1,  // 只有 flags
        SDS_TYPE_8 => @sizeOf(SdsHdr8),
        SDS_TYPE_16 => @sizeOf(SdsHdr16),
        SDS_TYPE_32 => @sizeOf(SdsHdr32),
        SDS_TYPE_64 => @sizeOf(SdsHdr64),
        else => unreachable,
    };
}

// ═══════════════════════════════════════════════════════════════════════════
// C ABI 兼容的导出函数
// ═══════════════════════════════════════════════════════════════════════════

/// 创建新的 SDS (C ABI)
export fn sdsnewlen(init: ?[*]const u8, initlen: usize) Sds {
    const allocator = getAllocator();

    const type_flags = sdsReqType(initlen);
    const hdrlen = hdrSize(type_flags);
    const total = hdrlen + initlen + 1;

    const sh = allocator.alloc(u8, total) catch return null;

    // 初始化头部
    switch (type_flags) {
        SDS_TYPE_8 => {
            const hdr = @as(*SdsHdr8, @ptrCast(sh.ptr));
            hdr.len = @intCast(initlen);
            hdr.alloc = @intCast(initlen);
            hdr.flags = SDS_TYPE_8;
        },
        SDS_TYPE_16 => {
            const hdr = @as(*SdsHdr16, @ptrCast(sh.ptr));
            hdr.len = @intCast(initlen);
            hdr.alloc = @intCast(initlen);
            hdr.flags = SDS_TYPE_16;
        },
        // ... 其他类型类似
        else => {},
    }

    const s = sh.ptr + hdrlen;
    if (initlen > 0) {
        if (init) |ptr| {
            @memcpy(s[0..initlen], ptr[0..initlen]);
        }
    }
    s[initlen] = 0;

    return s;
}

/// 释放 SDS (C ABI)
export fn sdsfree(s: Sds) void {
    if (s == null) return;
    const allocator = getAllocator();

    const flags = (s - 1)[0];
    const hdrlen = hdrSize(flags);
    const sh = s - hdrlen;

    allocator.free(sh[0..hdrlen]);  // 简化的释放，实际需要存储总大小
}

/// 获取长度 (C ABI)
export fn sdslen(s: Sds) usize {
    if (s == null) return 0;

    const flags = (s - 1)[0];
    return switch (flags & SDS_TYPE_MASK) {
        SDS_TYPE_8 => @as(*SdsHdr8, @ptrCast(@constCast(s - @sizeOf(SdsHdr8)))).len,
        SDS_TYPE_16 => @as(*SdsHdr16, @ptrCast(@constCast(s - @sizeOf(SdsHdr16)))).len,
        SDS_TYPE_32 => @as(*SdsHdr32, @ptrCast(@constCast(s - @sizeOf(SdsHdr32)))).len,
        SDS_TYPE_64 => @as(*SdsHdr64, @ptrCast(@constCast(s - @sizeOf(SdsHdr64)))).len,
        else => 0,
    };
}

// ═══════════════════════════════════════════════════════════════════════════
// Zig 风格的高级 API
// ═══════════════════════════════════════════════════════════════════════════

pub const SdsBuffer = struct {
    buf: []u8,
    hdr_len: usize,

    pub fn init(allocator: Allocator, initial: []const u8) !SdsBuffer {
        const type_flags = sdsReqType(initial.len);
        const hdrlen = hdrSize(type_flags);
        const total = hdrlen + initial.len + 1;

        const buf = try allocator.alloc(u8, total);

        // 初始化头部...

        const data_start = hdrlen;
        if (initial.len > 0) {
            @memcpy(buf[data_start..][0..initial.len], initial);
        }
        buf[data_start + initial.len] = 0;

        return SdsBuffer{
            .buf = buf,
            .hdr_len = hdrlen,
        };
    }

    pub fn deinit(self: *SdsBuffer, allocator: Allocator) void {
        allocator.free(self.buf);
        self.* = undefined;
    }

    pub fn data(self: SdsBuffer) [:0]u8 {
        const start = self.hdr_len;
        const len = self.len();
        return self.buf[start..][0..len :0];
    }

    pub fn len(self: SdsBuffer) usize {
        const flags = self.buf[self.hdr_len - 1];
        return switch (flags & SDS_TYPE_MASK) {
            SDS_TYPE_8 => @as(*SdsHdr8, @ptrCast(self.buf.ptr)).len,
            SDS_TYPE_16 => @as(*SdsHdr16, @ptrCast(self.buf.ptr)).len,
            // ...
            else => 0,
        };
    }

    pub fn append(self: *SdsBuffer, allocator: Allocator, data: []const u8) !void {
        const current_len = self.len();
        const current_alloc = self.alloc();

        if (current_len + data.len > current_alloc) {
            // 扩容
            const new_alloc = (current_len + data.len) * 2;
            const type_flags = sdsReqType(new_alloc);
            const new_hdrlen = hdrSize(type_flags);
            const new_total = new_hdrlen + new_alloc + 1;

            const new_buf = try allocator.realloc(self.buf, new_total);

            // 如果需要，迁移头部 (类型改变)
            if (new_hdrlen != self.hdr_len) {
                // 移动数据
                const data_start = new_hdrlen;
                @memmove(new_buf.ptr + data_start, new_buf.ptr + self.hdr_len, current_len);
                self.hdr_len = new_hdrlen;
            }

            self.buf = new_buf;
            // 更新头部 alloc...
        }

        const start = self.hdr_len + current_len;
        @memcpy(self.buf[start..][0..data.len], data);
        self.buf[self.hdr_len + current_len + data.len] = 0;

        // 更新 len...
    }

    fn alloc(self: SdsBuffer) usize {
        // 返回 alloc 字段...
        return 0;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// 测试
// ═══════════════════════════════════════════════════════════════════════════

test "sds basic operations" {
    const allocator = std.testing.allocator;
    setAllocator(allocator);

    // C ABI 测试
    const s = sdsnewlen("hello", 5);
    try std.testing.expectEqual(@as(usize, 5), sdslen(s));
    sdsfree(s);

    // Zig API 测试
    var buf = try SdsBuffer.init(allocator, "world");
    defer buf.deinit(allocator);
    try std.testing.expectEqualStrings("world", buf.data());
}
