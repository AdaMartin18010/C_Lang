//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 360
// Language: zig
// Block ID: f9b4402d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026年：改进的C头文件导入

// 自动生成绑定
c_import @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("string.h");
});

// 使用C标准库
const c = @cImport({
    @cInclude("my_c_library.h");
});

pub fn main() !void {
    const ptr = c.malloc(1024);
    defer c.free(ptr);

    // Zig自动处理类型转换
    const zig_slice = @as([*]u8, @ptrCast(ptr))[0..1024];
    @memset(zig_slice, 0);
}
