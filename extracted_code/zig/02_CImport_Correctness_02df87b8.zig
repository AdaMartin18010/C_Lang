//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\02_CImport_Correctness.md
// Line: 206
// Language: zig
// Block ID: 02df87b8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 生成的代码

// 结构体
pub const struct_Node = extern struct {
    value: c_int,
    next: ?*struct_Node,
};
pub const Node = struct_Node;

// 联合体
pub const union_Data = extern union {
    i: c_int,
    f: f32,
    bytes: [4]u8,
};
pub const Data = union_Data;

// 枚举
pub const enum_Color = c_int;
pub const RED: c_int = 0;
pub const GREEN: c_int = 1;
pub const BLUE: c_int = 2;
pub const Color = enum_Color;

// 函数指针
pub const callback_t = ?*const fn (c_int, ?*anyopaque) callconv(.C) c_int;

// 复杂声明
pub extern fn signal(sig: c_int, func: ?*const fn (c_int) callconv(.C) c_int)
    ?*const fn (c_int) callconv(.C) c_int;
