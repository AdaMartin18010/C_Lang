//
// Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\README.md
// Line: 299
// Language: zig
// Block ID: 4047993f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// 示例：将 C 的配置解析器替换为 Zig 实现
/// 阶段 1: 保持相同接口

// src/config/legacy_adapter.zig - 第一阶段：直接包装
const c = @import("../ffi/c_bindings.zig").c;

pub const Config = struct {
    c_handle: *c.Config,

    pub fn load(path: []const u8) !Config {
        const c_path = try std.heap.c_allocator.dupeZ(u8, path);
        defer std.heap.c_allocator.free(c_path);

        var handle: ?*c.Config = null;
        const status = c.config_load(c_path.ptr, &handle);
        try statusToError(status);

        return Config{ .c_handle = handle.? };
    }

    pub fn get(self: Config, key: []const u8) ?[]const u8 {
        const c_key = std.heap.c_allocator.dupeZ(u8, key) catch return null;
        defer std.heap.c_allocator.free(c_key);

        const value = c.config_get(self.c_handle, c_key.ptr);
        if (value == null) return null;
        return std.mem.span(value);
    }

    pub fn deinit(self: Config) void {
        c.config_free(self.c_handle);
    }
};
