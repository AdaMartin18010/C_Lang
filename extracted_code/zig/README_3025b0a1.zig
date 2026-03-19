//
// Auto-generated from: Zig\README.md
// Line: 848
// Language: zig
// Block ID: 3025b0a1
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cInclude("stdlib.h");
});

pub fn main() !void {
    // 分配 C 内存
    const ptr = c.malloc(100);
    if (ptr == null) return error.OutOfMemory;
    defer c.free(ptr);

    // 使用指针
    const bytes = @as([*]u8, @ptrCast(ptr));
    bytes[0] = 'H';
    bytes[1] = 'i';
    bytes[2] = 0;
}
