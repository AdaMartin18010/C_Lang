//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\README.md
// Line: 37
// Language: zig
// Block ID: 61ae5525
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

pub fn main() !void {
    _ = c.printf("Hello from Zig!\n");

    const ptr = c.malloc(1024);
    defer c.free(ptr);

    _ = c.memset(ptr, 0, 1024);
}
