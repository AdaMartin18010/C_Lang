//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
// Line: 298
// Language: zig
// Block ID: bee9bc60
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// C 字符串互操作
const c = @cImport({
    @cInclude("string.h");
    @cInclude("stdlib.h");
});
const std = @import("std");

pub fn stringDemo(allocator: std.mem.Allocator) void {
    // Zig 字符串字面量自动转换为 C 字符串
    const c_str = "Hello, C!";  // [*c]const u8 类型

    // Zig 字符串切片转换为 C 字符串
    const zig_str: []const u8 = "Hello, Zig!";
    const c_ptr: [*c]const u8 = zig_str.ptr;  // 注意: 需要确保空终止

    // 安全转换: 复制到空终止缓冲区
    var c_buffer = allocator.allocSentinel(u8, zig_str.len, 0) catch unreachable;
    defer allocator.free(c_buffer);
    @memcpy(c_buffer.ptr, zig_str.ptr, zig_str.len);

    // 使用 C 字符串函数
    const len = c.strlen(c_buffer.ptr);
    std.debug.print("Length: {d}\n", .{len});
}
