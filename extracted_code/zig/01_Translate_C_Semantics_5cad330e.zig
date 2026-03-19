//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 476
// Language: zig
// Block ID: 5cad330e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const uint32 = c_uint;
pub const Point = extern struct {
    // ... 字段
};
pub const callback_t = ?*const fn (c_int, ?*anyopaque) callconv(.C) void;
pub const array_t = [10]c_int;
