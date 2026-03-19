//
// Auto-generated from: Zig\Zig_C_Interoperability.md
// Line: 109
// Language: zig
// Block ID: 61e841f3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 导入标准 C 库
const c = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdlib.h");
    @cInclude("stdio.h");
    @cInclude("string.h");
});

pub fn main() !void {
    // 使用 C 标准库函数
    const ptr = c.malloc(1024);
    defer c.free(ptr);

    _ = c.printf("Hello from Zig!\n");

    var buf: [256]u8 = undefined;
    _ = c.snprintf(&buf, buf.len, "Number: %d", 42);
}
