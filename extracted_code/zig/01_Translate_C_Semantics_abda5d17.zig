//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 627
// Language: zig
// Block ID: abda5d17
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const union_Data = extern union {
    i: c_int,
    f: f32,
    bytes: [4]u8,
};

pub const Data = union_Data;

pub const union_AlignedData = extern union {
    c: u8,
    i: c_int,
};

// 对齐属性需要手动处理
pub const AlignedData = union_AlignedData;
