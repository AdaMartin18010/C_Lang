//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 179
// Language: zig
// Block ID: d7893e11
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 导入标准C库
const c = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

// 使用C函数
pub fn main() void {
    const msg = "Hello from C\n";
    _ = c.printf("%s", msg);

    const ptr = c.malloc(100);
    defer c.free(ptr);

    _ = c.strcpy(@ptrCast(ptr), "copied");
}
