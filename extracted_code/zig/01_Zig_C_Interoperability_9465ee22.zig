//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 335
// Language: zig
// Block ID: 9465ee22
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 回调与 Zig

const c = @cImport({
    @cInclude("callback_api.h");
});

// C 回调类型: typedef void (*callback_t)(int data, void* user_data);

// Zig 回调函数
fn my_callback(data: c_int, user_data: ?*anyopaque) callconv(.C) void {
    const context = @as(*Context, @alignCast(@ptrCast(user_data.?)));
    context.handle(@intCast(data));
}

const Context = struct {
    count: u32,

    fn handle(self: *Context, data: u32) void {
        self.count += data;
    }
};

pub fn register_callback() void {
    var context = Context{ .count = 0 };

    // 注册回调
    c.register_callback(my_callback, &context);
}

// 使用闭包模式
fn CallbackClosure(comptime T: type) type {
    return struct {
        context: *T,
        callback: *const fn (*T, c_int) void,

        pub fn invoke(ctx: ?*anyopaque, data: c_int) callconv(.C) void {
            const self = @as(*@This(), @alignCast(@ptrCast(ctx.?)));
            self.callback(self.context, data);
        }
    };
}
