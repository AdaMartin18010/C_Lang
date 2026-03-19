//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 136
// Language: zig
// Block ID: 95ab8de9
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");
const builtin = @import("builtin");

/// 根据平台自动选择正确的调用约定
pub const CallConv = switch (builtin.target.os.tag) {
    .windows => if (builtin.target.cpu.arch == .x86)
        .Stdcall
    else
        .C,
    else => .C,
};

/// 跨平台回调类型定义
pub const CallbackFn = *const fn (ctx: ?*anyopaque, data: []const u8) callconv(CallConv) void;

/// 导出函数使用平台特定调用约定
export fn register_callback(callback: CallbackFn, ctx: ?*anyopaque) callconv(CallConv) i32 {
    _ = callback;
    _ = ctx;
    return 0;
}
