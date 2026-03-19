//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 923
// Language: zig
// Block ID: 72e69260
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const struct_AlignedStruct = extern struct {
    c: u8,
    // 隐式填充: 15 bytes
    x: i32 align(16),
    // 填充: 4 bytes
    d: f64,
};

pub const struct_CacheLine align(32) = extern struct {
    data: [64]u8,
};
