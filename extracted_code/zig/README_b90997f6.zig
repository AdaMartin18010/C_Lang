//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 57
// Language: zig
// Block ID: b90997f6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// translate-c 生成的典型输出结构
const @"cImport" = @cImport({
    @cDefine("_GNU_SOURCE", "");
    @cInclude("stdlib.h");
});

// 宏被转换为内联函数或常量
pub const EXIT_SUCCESS = @as(c_int, 0);
pub const EXIT_FAILURE = @as(c_int, 1);

// 函数声明
pub extern fn malloc(__size: c_ulong) ?*anyopaque;
pub extern fn free(__ptr: ?*anyopaque) void;

// 结构体定义
pub const Struct_timespec = extern struct {
    tv_sec: c_long,
    tv_nsec: c_long,
};
