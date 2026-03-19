//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 696
// Language: zig
// Block ID: 2f43decc
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码 (Zig 0.14+)
pub const struct_HardwareRegister = packed struct {
    enable: u1,
    mode: u3,
    _: u4,                    // 匿名填充
    status: u8,
    data: u16,
};

// 替代方案: 使用位操作函数
pub const struct_HardwareRegister = extern struct {
    _bitfield1: u8,
    _bitfield2: u16,

    pub fn enable(self: @This()) bool {
        return (self._bitfield1 & 0x01) != 0;
    }

    pub fn set_enable(self: *@This(), value: bool) void {
        if (value) {
            self._bitfield1 |= 0x01;
        } else {
            self._bitfield1 &= ~@as(u8, 0x01);
        }
    }

    pub fn mode(self: @This()) u3 {
        return @truncate((self._bitfield1 >> 1) & 0x07);
    }

    pub fn status(self: @This()) u8 {
        return self._bitfield2 & 0xFF;
    }

    pub fn data(self: @This()) u16 {
        return (self._bitfield2 >> 8) | (@as(u16, self._bitfield3) << 8);
    }
};
