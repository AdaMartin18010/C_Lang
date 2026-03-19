//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 521
// Language: zig
// Block ID: 32f12cfb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const struct_Point = extern struct {
    x: c_int,
    y: c_int,
};

pub const Point = struct_Point;

pub const struct_Rectangle = extern struct {
    top_left: Point,
    bottom_right: Point,
};

pub const Rectangle = struct_Rectangle;
