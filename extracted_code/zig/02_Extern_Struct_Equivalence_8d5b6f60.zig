//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 374
// Language: zig
// Block ID: 8d5b6f60
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - 相同结构的 extern struct
const std = @import("std");

const Padded = extern struct {
    a: u8,
    b: c_int,
    c: u8,
};

const Optimized = extern struct {
    b: c_int,
    a: u8,
    c: u8,
};

const Mixed = extern struct {
    a: u8,
    d: f64,
    b: u8,
    i: c_int,
};

pub fn main() void {
    std.debug.print("@sizeOf(Padded)    = {d}\n", .{@sizeOf(Padded)});
    std.debug.print("@sizeOf(Optimized) = {d}\n", .{@sizeOf(Optimized)});
    std.debug.print("@sizeOf(Mixed)     = {d}\n", .{@sizeOf(Mixed)});

    // 详细的填充分析
    std.debug.print("\nPadded layout:\n", .{});
    std.debug.print("  a @ {d} (size {d})\n", .{@offsetOf(Padded, "a"), @sizeOf(u8)});
    std.debug.print("  b @ {d} (size {d})\n", .{@offsetOf(Padded, "b"), @sizeOf(c_int)});
    std.debug.print("  c @ {d} (size {d})\n", .{@offsetOf(Padded, "c"), @sizeOf(u8)});
}
