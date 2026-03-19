//
// Auto-generated from: Zig\Zig_C_Interoperability.md
// Line: 477
// Language: zig
// Block ID: c9a5e482
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// safe_api.zig - 安全的 C API

const std = @import("std");

// 不透明句柄
pub const DatabaseHandle = *opaque {};

// 错误码
pub const ErrorCode = enum(c_int) {
    success = 0,
    null_pointer = -1,
    invalid_argument = -2,
    out_of_memory = -3,
    io_error = -4,
};

// 创建数据库
export fn database_create(out_handle: ?*?*DatabaseHandle) callconv(.C) ErrorCode {
    const handle = out_handle orelse return .null_pointer;

    const db = std.heap.c_allocator.create(Database) catch return .out_of_memory;
    db.* = Database.init();

    handle.* = @ptrCast(db);
    return .success;
}

// 销毁数据库
export fn database_destroy(handle: ?*DatabaseHandle) callconv(.C) ErrorCode {
    const db = @as(?*Database, @ptrCast(@alignCast(handle))) orelse return .null_pointer;

    db.deinit();
    std.heap.c_allocator.destroy(db);

    return .success;
}

// 执行查询
export fn database_query(
    handle: ?*DatabaseHandle,
    query: ?[*:0]const u8,
    out_result: ?*?[*:0]u8,
) callconv(.C) ErrorCode {
    const db = @as(?*Database, @ptrCast(@alignCast(handle))) orelse return .null_pointer;
    const q = query orelse return .null_pointer;
    const result = out_result orelse return .null_pointer;

    const result_str = db.query(std.mem.span(q)) catch return .io_error;

    const c_str = std.heap.c_allocator.dupeZ(u8, result_str) catch return .out_of_memory;
    result.* = c_str.ptr;

    return .success;
}

// 释放查询结果
export fn database_free_result(result: ?[*:0]u8) callconv(.C) void {
    if (result) |r| {
        std.heap.c_allocator.free(std.mem.span(r));
    }
}

// 内部实现
const Database = struct {
    data: std.StringHashMap([]const u8),

    fn init() Database {
        return .{
            .data = std.StringHashMap([]const u8).init(std.heap.c_allocator),
        };
    }

    fn deinit(self: *Database) void {
        self.data.deinit();
    }

    fn query(self: *Database, q: []const u8) ![]const u8 {
        return self.data.get(q) orelse "NOT_FOUND";
    }
};
