//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\README.md
// Line: 131
// Language: zig
// Block ID: e7b5f9a4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 为不安全的C库创建类型安全包装
const raw = @cImport(@cInclude("legacy_lib.h"));

pub const SafeHandle = struct {
    raw_handle: *raw.LegacyHandle,

    pub fn open(path: []const u8) !SafeHandle {
        const c_path = try std.heap.c_allocator.dupeZ(u8, path);
        defer std.heap.c_allocator.free(c_path);

        const h = raw.legacy_open(c_path.ptr) orelse return error.OpenFailed;
        return SafeHandle{ .raw_handle = h };
    }

    pub fn close(self: *SafeHandle) void {
        raw.legacy_close(self.raw_handle);
        self.raw_handle = undefined;
    }
};
