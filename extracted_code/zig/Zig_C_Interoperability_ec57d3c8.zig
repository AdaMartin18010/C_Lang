//
// Auto-generated from: Zig\Zig_C_Interoperability.md
// Line: 731
// Language: zig
// Block ID: ec57d3c8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 资源管理器
pub fn CResource(comptime T: type, comptime deinit_fn: fn (*T) callconv(.C) void) type {
    return struct {
        ptr: *T,

        pub fn init(ptr: *T) @This() {
            return .{ .ptr = ptr };
        }

        pub fn deinit(self: @This()) void {
            deinit_fn(self.ptr);
        }
    };
}

// 使用
const FileHandle = CResource(c.FILE, c.fclose);

pub fn read_file(path: []const u8) ![]u8 {
    const file = c.fopen(path.ptr, "r") orelse return error.FileNotFound;
    var handle = FileHandle.init(file);
    defer handle.deinit();

    // 读取文件...
    return &[_]u8{};
}
