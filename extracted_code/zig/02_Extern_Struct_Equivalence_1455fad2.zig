//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 2191
// Language: zig
// Block ID: 1455fad2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - library_binding.zig
const std = @import("std");

// 导入 C 库
const c = @cImport({
    @cInclude("library.h");
});

// 或使用直接 Zig 映射
pub const ContextPtr = *opaque {};

pub const Options = extern struct {
    version: u32,
    flags: u32,
    timeout_ms: u16,
    retry_count: u16,
    buffer_size: u32,
    config_path: ?[*:0]const u8,
};

pub const Result = extern struct {
    code: i32,
    data_len: u32,
    data: ?[*]const u8,
};

pub const DataCallback = *const fn (*const Result, ?*anyopaque) callconv(.C) void;
pub const LogCallback = *const fn (c_int, [*:0]const u8) callconv(.C) void;

// 外部函数声明
pub extern fn context_create(options: *const Options) ?ContextPtr;
pub extern fn context_destroy(ctx: ?ContextPtr) void;
pub extern fn context_execute(
    ctx: ?ContextPtr,
    input: ?[*]const u8,
    len: u32,
    cb: ?DataCallback,
    user_data: ?*anyopaque,
) c_int;
pub extern fn set_log_callback(cb: ?LogCallback) void;
pub extern fn get_version() [*:0]const u8;
pub extern fn get_api_version() u32;

// 安全的 Zig 包装器
pub const Context = struct {
    ptr: ?ContextPtr,

    pub const Error = error{
        NullContext,
        CreateFailed,
        ExecutionFailed,
    };

    pub fn init(opts: *const Options) Error!Context {
        const ptr = context_create(opts);
        if (ptr == null) return Error.CreateFailed;
        return .{ .ptr = ptr };
    }

    pub fn deinit(self: *Context) void {
        if (self.ptr) |p| {
            context_destroy(p);
            self.ptr = null;
        }
    }

    pub fn execute(
        self: Context,
        input: []const u8,
        comptime Callback: type,
        callback: Callback,
        user_data: ?*anyopaque,
    ) Error!void {
        if (self.ptr == null) return Error.NullContext;

        const cb = struct {
            fn wrapper(result: *const Result, ud: ?*anyopaque) callconv(.C) void {
                const cb_ptr: *const Callback = @ptrCast(@alignCast(ud));
                cb_ptr(result);
            }
        }.wrapper;

        const rc = context_execute(
            self.ptr,
            input.ptr,
            @intCast(input.len),
            cb,
            user_data,
        );

        if (rc != 0) return Error.ExecutionFailed;
    }
};

// 验证 FFI 兼容性
comptime {
    std.debug.assert(@sizeOf(Options) == @sizeOf(c.struct_Options));
    std.debug.assert(@offsetOf(Options, "flags") == @offsetOf(c.struct_Options, "flags"));
    std.debug.assert(@sizeOf(Result) == @sizeOf(c.struct_Result));
}

pub fn main() void {
    std.debug.print("FFI bindings loaded\n", .{});
    std.debug.print("API version: {d}\n", .{get_api_version()});
    std.debug.print("Version: {s}\n", .{get_version()});
}
