//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 557
// Language: zig
// Block ID: f555a220
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const struct_PackedHeader = packed struct {
    type: u8,
    length: u16,
    checksum: u32,
};

pub const struct_PackedData = packed struct {
    id: u8,
    value: c_int,
    data: f64,
};
