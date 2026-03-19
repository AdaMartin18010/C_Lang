//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 127
// Language: zig
// Block ID: 8b1513f9
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 代码:
/// enum Color { RED = 0, GREEN, BLUE };
/// typedef enum Color Color;

// Zig 枚举 - 更严格的类型安全
pub const Color = enum(c_int) {
    red = 0,
    green,
    blue,

    pub const RED = .red;    // 兼容性别名
    pub const GREEN = .green;
    pub const BLUE = .blue;
};

/// C 代码:
/// typedef union {
///     int i;
///     float f;
///     char c[4];
/// } Data;

// Zig 联合类型 - 需要标记联合增加安全性
pub const DataTag = enum {
    int,
    float,
    bytes,
};

pub const Data = union(DataTag) {
    int: i32,
    float: f32,
    bytes: [4]u8,
};

// 如果需要 C 兼容的裸联合
pub const CData = extern union {
    i: i32,
    f: f32,
    c: [4]u8,
};
