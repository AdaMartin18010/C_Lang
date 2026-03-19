//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1219
// Language: zig
// Block ID: eb4f7080
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub extern var generic_ptr: ?*anyopaque;
pub extern var int_array: [*c]c_int;
pub extern var const_string: [*c]const u8;
pub extern var mutable_string: [*c]u8;
pub extern var func_ptr: ?*const fn (c_int, c_int) callconv(.C) c_int;
pub extern var returns_ptr: ?*const fn () callconv(.C) [*c]c_int;
pub extern var takes_ptr: ?*const fn ([*c]c_int) callconv(.C) void;
pub extern var pointer_to_pointer: [*c][*c]c_int;
pub extern var array_of_ptrs: [*c][10]c_int;
pub extern var ptr_to_array: [10][*c]c_int;
