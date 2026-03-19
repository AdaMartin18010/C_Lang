//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 1386
// Language: zig
// Block ID: 39c3a989
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 变参函数互操作

// Zig 本身不支持 C 风格的变参函数
// 但可以通过 extern fn 调用 C 变参函数
extern "c" fn printf(format: [*:0]const u8, ...) c_int;
extern "c" fn sprintf(buf: [*]u8, format: [*:0]const u8, ...) c_int;

// 包装函数示例
pub fn logInfo(comptime fmt: []const u8, args: anytype) void {
    // Zig 的编译时泛型替代变参
    const msg = std.fmt.allocPrint(std.heap.page_allocator, fmt, args) catch return;
    defer std.heap.page_allocator.free(msg);

    // 调用 C printf
    _ = printf("[INFO] %s\n", msg.ptr);
}

// 如果必须实现 C 可调用的变参函数，
// 需要在 C 中包装一层
