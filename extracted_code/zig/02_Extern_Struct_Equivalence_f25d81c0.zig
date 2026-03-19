//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 484
// Language: zig
// Block ID: f25d81c0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 对齐控制
const std = @import("std");

// 结构体级对齐
const AlignedStruct = extern struct {
    x: c_int,
};
// Zig 中 extern struct 不能直接用 align() 修饰整个结构体
// 但可以通过其他方式控制

// 字段级对齐 - Zig 使用 align 修饰符
const FieldAlign = extern struct {
    c: u8,
    i: c_int align(8),  // i 从 8 字节边界开始
};

// packed struct (1 字节对齐)
const PackedStruct = packed struct {
    c: u8,
    i: c_int,  // 在 offset 1，可能非对齐
};

// 显式指定结构体对齐
const ForcedAlign = extern struct {
    data: [4]f64,
    pub const alignment = 32;  // 自定义对齐
};

pub fn main() void {
    std.debug.print("@alignOf(FieldAlign) = {d}\n", .{@alignOf(FieldAlign)});
    std.debug.print("@alignOf(PackedStruct) = {d}\n", .{@alignOf(PackedStruct)});
    std.debug.print("@offsetOf(FieldAlign, \"i\") = {d}\n", .{@offsetOf(FieldAlign, "i")});
}
