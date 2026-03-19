//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 987
// Language: zig
// Block ID: efa2a274
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const struct_WithPadding = extern struct {
    a: u8,
    // Zig 不直接暴露填充字节
    b: c_int,
    c: u8,
    // 更多填充
    d: f64,
};

// 验证布局
comptime {
    assert(@sizeOf(struct_WithPadding) == 24);
    assert(@offsetOf(struct_WithPadding, "a") == 0);
    assert(@offsetOf(struct_WithPadding, "b") == 4);
    assert(@offsetOf(struct_WithPadding, "c") == 8);
    assert(@offsetOf(struct_WithPadding, "d") == 16);
}
