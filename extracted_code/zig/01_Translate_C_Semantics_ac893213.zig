//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 584
// Language: zig
// Block ID: ac893213
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub const struct_StatusFlags = extern struct {
    // 位域使用 packed struct 和位宽类型模拟
    bitfields: packed struct {
        is_active: u1,
        priority: u3,
        category: u4,
        reserved: u24,
    },
};

// 或者使用手动位操作（更常见）
pub const struct_StatusFlags = extern struct {
    _bitfield: c_uint,

    pub fn is_active(self: @This()) bool {
        return (self._bitfield & 0x1) != 0;
    }

    pub fn priority(self: @This()) u3 {
        return @truncate((self._bitfield >> 1) & 0x7);
    }
    // ...
};
