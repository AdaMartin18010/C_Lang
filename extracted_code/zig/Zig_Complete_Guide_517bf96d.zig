//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 1068
// Language: zig
// Block ID: 517bf96d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

// 导入 C 标准库
const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

// 导入自定义 C 库
const mylib = @cImport({
    @cInclude("mylib.h");
});

pub fn main() void {
    // 使用 C 函数
    _ = c.printf("Hello from C printf!\n");

    // C 内存分配
    const ptr = c.malloc(100);
    defer c.free(ptr);

    _ = c.strcpy(@as([*c]u8, @ptrCast(ptr)), "Zig + C");
    _ = c.printf("String: %s\n", @as([*c]const u8, @ptrCast(ptr)));

    // 调用自定义 C 函数
    const result = mylib.add(5, 3);
    _ = c.printf("Result: %d\n", result);
}

// 导出 Zig 函数给 C 使用
pub export fn zig_add(a: i32, b: i32) i32 {
    return a + b;
}

pub export fn zig_process(data: [*c]u8, len: usize) void {
    const slice = data[0..len];
    for (slice) |*byte| {
        byte.* = std.ascii.toUpper(byte.*);
    }
}

// 处理 C 结构体
const CStruct = extern struct {
    x: i32,
    y: i32,
    name: [32]u8,
};

pub export fn zig_process_struct(s: *CStruct) void {
    s.x += 10;
    s.y += 20;
    _ = std.fmt.bufPrint(&s.name, "Zig_{s}", .{s.name}) catch {};
}
