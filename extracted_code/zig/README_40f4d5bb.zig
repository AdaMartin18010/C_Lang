//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
// Line: 339
// Language: zig
// Block ID: 40f4d5bb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// migrate_example.zig - 渐进式迁移
const std = @import("std");
const c = @cImport({
    @cInclude("legacy_api.h");
});

// 1. 使用Zig包装C代码
pub const CLegacy = struct {
    handle: *c.legacy_context,

    pub fn init() !CLegacy {
        var handle: *c.legacy_context = undefined;
        const err = c.legacy_init(&handle);
        if (err != 0) return error.InitFailed;
        return CLegacy{ .handle = handle };
    }

    pub fn deinit(self: *CLegacy) void {
        c.legacy_cleanup(self.handle);
    }

    // 包装不安全C接口为安全Zig接口
    pub fn process(self: *CLegacy, data: []const u8) !void {
        const err = c.legacy_process(
            self.handle,
            data.ptr,
            @intCast(c_int, data.len)
        );
        if (err != 0) return error.ProcessFailed;
    }
};

// 2. 逐步重写关键模块
pub fn modernImplementation(data: []const u8) ![]const u8 {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();

    var result = std.ArrayList(u8).init(gpa.allocator());
    defer result.deinit();

    for (data) |byte| {
        try result.append(byte);
    }

    return result.toOwnedSlice();
}
