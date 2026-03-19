//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 148
// Language: zig
// Block ID: 16245fcf
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// ffi/memory_bridge.zig - 跨边界内存管理
const std = @import("std");
const c = @import("c_bindings.zig").c;

/// 内存所有权约定:
/// - C 分配的内存必须由 C 释放
/// - Zig 分配的内存必须由 Zig 释放
/// - 跨边界传递时，必须明确所有权

/// 从 C 获取数据，复制到 Zig 管理
pub fn copyFromC(c_ptr: ?[*]const u8, len: usize, allocator: std.mem.Allocator) ![]u8 {
    if (c_ptr == null) return error.NullPointer;

    const copy = try allocator.alloc(u8, len);
    errdefer allocator.free(copy);

    @memcpy(copy, c_ptr.?[0..len]);
    return copy;
}

/// 将 Zig 数据传递给 C（C 负责释放）
pub fn passToC(data: []const u8, c_allocator: *const fn (usize) callconv(.C) ?*anyopaque) !?*anyopaque {
    const c_mem = c_allocator(data.len) orelse return error.OutOfMemory;
    @memcpy(@as([*]u8, @ptrCast(c_mem))[0..data.len], data);
    return c_mem;
}

/// 安全的 RAII 包装器
pub fn CLegacyResource(comptime T: type, comptime deinit_fn: anytype) type {
    return struct {
        ptr: *T,

        const Self = @This();

        pub fn init(ptr: *T) Self {
            return .{ .ptr = ptr };
        }

        pub fn deinit(self: Self) void {
            deinit_fn(self.ptr);
        }
    };
}
