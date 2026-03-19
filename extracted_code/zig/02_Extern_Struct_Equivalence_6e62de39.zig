//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 193
// Language: zig
// Block ID: 6e62de39
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - extern struct 布局保证
const std = @import("std");

const Example = extern struct {
    c: u8,       // 1 byte
    i: c_int,    // 4 bytes (平台相关)
    s: c_short,  // 2 bytes
};

// 内存布局分析与 C 完全一致:
// offset 0:  u8 c       [1 byte]
// offset 1:  padding    [3 bytes]
// offset 4:  c_int i    [4 bytes]
// offset 8:  c_short s  [2 bytes]
// offset 10: padding    [2 bytes]
// Total: 12 bytes

pub fn main() void {
    std.debug.print("sizeof(Example) = {d}\n", .{@sizeOf(Example)});
    std.debug.print("offsetof(c) = {d}\n", .{@offsetOf(Example, "c")});
    std.debug.print("offsetof(i) = {d}\n", .{@offsetOf(Example, "i")});
    std.debug.print("offsetof(s) = {d}\n", .{@offsetOf(Example, "s")});
}
