//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 363
// Language: zig
// Block ID: 9dc25d41
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cInclude("callback_lib.h");
});

// 定义C调用约定的回调函数
fn myCallback(data: ?*anyopaque, value: c_int) callconv(.C) void {
    const ctx = @as(*Context, @ptrCast(@alignCast(data)));
    ctx.sum += value;
    std.debug.print("Callback: value={}, sum={}\n", .{value, ctx.sum});
}

const Context = struct {
    sum: c_int = 0,
};

pub fn main() void {
    var ctx = Context{};

    // 注册回调
    c.register_callback(&ctx, myCallback);

    // 触发回调 (假设C库会在某些事件时调用)
    c.trigger_event();
}
