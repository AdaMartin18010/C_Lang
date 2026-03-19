//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 607
// Language: zig
// Block ID: f5ee37e0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - packed struct 位域
const std = @import("std");

// packed struct 提供确定性的位布局
const BitField = packed struct {
    a: u4,   // 4 bits
    b: u8,   // 8 bits
    c: u12,  // 12 bits
    d: u8,   // 8 bits
};
// 总大小: 4 + 8 + 12 + 8 = 32 bits = 4 bytes

// 确定性的位布局: 字段从 LSB 开始
// Bit layout: [cccc cccc dddd dddd cccc bbbb bbbb aaaa]
//              31..24   23..16   15..12 11..4   3..0

// 有符号位域
const SignedBitField = packed struct {
    x: i3,  // -4 到 3
    y: i3,  // -4 到 3
};

// 结合整数类型的 packed struct
const MixedPacked = packed struct {
    flag1: u1,
    flag2: u1,
    reserved: u6,
    data: u8,
};

// 位操作辅助函数
fn setBitA(bf: *BitField, value: u4) void {
    bf.a = value;
}

fn getBitD(bf: BitField) u8 {
    return bf.d;
}

pub fn main() void {
    std.debug.print("@sizeOf(BitField) = {d}\n", .{@sizeOf(BitField)});

    var bf: BitField = .{
        .a = 10,
        .b = 200,
        .c = 3000,
        .d = 128,
    };

    // 查看底层字节表示
    const bytes = std.mem.asBytes(&bf);
    std.debug.print("Raw bytes: ", .{});
    for (bytes) |b| {
        std.debug.print("{x:0>2} ", .{b});
    }
    std.debug.print("\n", .{});

    // 验证值
    std.debug.print("a={d}, b={d}, c={d}, d={d}\n",
        .{bf.a, bf.b, bf.c, bf.d});
}
