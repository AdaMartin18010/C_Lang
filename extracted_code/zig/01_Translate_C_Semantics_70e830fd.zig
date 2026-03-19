//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 1279
// Language: zig
// Block ID: 70e830fd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub extern fn array_param(arr: [*c]c_int) void;
pub extern fn array_param2(arr: [*c]c_int) void;
pub extern fn array_param3(arr: [*c][10]c_int) void;
// static 数组提示在 Zig 中没有直接对应

pub extern var global_array: [10]c_int;
pub extern var extern_array: [*c]c_int;  // 未知大小的数组
