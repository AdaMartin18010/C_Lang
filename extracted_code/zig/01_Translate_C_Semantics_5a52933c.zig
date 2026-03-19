//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1332
// Language: zig
// Block ID: 5a52933c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
// signal: 函数，接受 (int, 函数指针) 返回函数指针
pub extern fn signal(
    sig: c_int,
    func: ?*const fn (c_int) callconv(.C) void
) ?*const fn (c_int) callconv(.C) void;

// fp: 指向函数的指针，该函数接受 int，返回指向函数的指针，
//     该函数接受 void，返回指向函数的指针，该函数接受 int 返回 int
pub extern var fp: ?*const fn (c_int) callconv(.C) ?*const fn () callconv(.C) ?*const fn (c_int) callconv(.C) c_int;

// array: 数组，元素为指向函数的指针，该函数接受 int，返回指向数组的指针
pub extern var array: [10]?*const fn (c_int) callconv(.C) [*c][5][*c]u8;

// complicated: 复杂的 volatile 指针
pub extern var complicated: ?*volatile ?*const [10]?*const [*c]const [*c]const c_int;
