//
// Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
// Line: 1916
// Language: zig
// Block ID: ee248cdd
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码 - basic_struct.zig
const std = @import("std");

// 基础点结构体 - 与 C 完全等价
const Point = extern struct {
    x: i32,
    y: i32,
};

// 3D 点
const Point3D = extern struct {
    base: Point,
    z: i32,
};

// 矩形
const Rect = extern struct {
    top_left: Point,
    bottom_right: Point,
};

// 颜色
const Color = extern struct {
    r: u8,
    g: u8,
    b: u8,
    a: u8,
};

// 混合类型
const Mixed = extern struct {
    type: u8,
    flags: u16,
    data: u32,
    timestamp: f64,
};

// 验证与 C 的等价性
comptime {
    // 大小验证
    std.debug.assert(@sizeOf(Point) == 8);
    std.debug.assert(@sizeOf(Point3D) == 12);
    std.debug.assert(@sizeOf(Rect) == 16);
    std.debug.assert(@sizeOf(Color) == 4);
    std.debug.assert(@sizeOf(Mixed) == 24);

    // 偏移验证
    std.debug.assert(@offsetOf(Point3D, "z") == 8);
    std.debug.assert(@offsetOf(Rect, "bottom_right") == 8);
    std.debug.assert(@offsetOf(Mixed, "timestamp") == 16);
}

pub fn main() void {
    std.debug.print("Point: size={d}, align={d}\n", .{@sizeOf(Point), @alignOf(Point)});
    std.debug.print("Point3D: size={d}, align={d}\n", .{@sizeOf(Point3D), @alignOf(Point3D)});
    std.debug.print("Rect: size={d}, align={d}\n", .{@sizeOf(Rect), @alignOf(Rect)});
    std.debug.print("Color: size={d}, align={d}\n", .{@sizeOf(Color), @alignOf(Color)});
    std.debug.print("Mixed: size={d}, align={d}\n", .{@sizeOf(Mixed), @alignOf(Mixed)});
}
