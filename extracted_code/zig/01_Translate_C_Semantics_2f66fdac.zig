//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1882
// Language: zig
// Block ID: 2f66fdac
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 最佳实践 1: 手动改进生成的绑定

// translate-c 生成的代码 (可能不够精确)
pub extern fn malloc(size: c_ulong) ?*anyopaque;

// 改进后的 Zig 包装
pub fn malloc(comptime T: type, count: usize) ?[*]T {
    const total_size = @sizeOf(T) * count;
    const ptr = std.c.malloc(total_size);
    return @ptrCast(ptr);
}

// 最佳实践 2: 使用 opaque 处理不透明类型
// C: typedef struct Handle Handle;
pub const Handle = opaque {};

// 更好的方式 - 添加方法
pub const Handle = opaque {
    pub extern fn create() ?*Handle;
    pub extern fn destroy(self: *Handle) void;
    pub extern fn doWork(self: *Handle, data: i32) c_int;
};

// 最佳实践 3: 错误处理转换
// C 函数返回错误码
pub extern fn open_file(path: [*c]const u8, handle: [*c]?*FileHandle) c_int;

// Zig 包装
pub const FileError = error{
    NotFound,
    PermissionDenied,
    Unknown,
};

pub fn openFile(path: []const u8) FileError!*FileHandle {
    var handle: ?*FileHandle = null;
    const c_path = try std.cstr.addNullByte(allocator, path);
    defer allocator.free(c_path);

    const result = open_file(c_path.ptr, &handle);
    return switch (result) {
        0 => handle.?,
        -1 => error.NotFound,
        -2 => error.PermissionDenied,
        else => error.Unknown,
    };
}

// 最佳实践 4: 回调函数包装
// C 风格的回调
pub const ProgressCallback = ?*const fn (c_int, c_int, ?*anyopaque) callconv(.C) void;

// Zig 友好的 API
pub const ProgressFn = *const fn (current: i32, total: i32) void;

pub fn registerProgressCallback(ctx: ?*anyopaque, comptime callback: ProgressFn) ProgressCallback {
    return struct {
        fn wrapper(current: c_int, total: c_int, user_data: ?*anyopaque) callconv(.C) void {
            _ = user_data;
            callback(@intCast(current), @intCast(total));
        }
    }.wrapper;
}

// 最佳实践 5: 使用 @cImport 处理标准头文件
// 对于标准库头文件，使用 @cImport 更方便
const c = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdlib.h");
    @cInclude("string.h");
    @cInclude("math.h");
});

// 对于自定义头文件，生成的 Zig 代码更可控
// zig translate-c my_header.h > my_header.zig
