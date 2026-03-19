//
// Auto-generated from: Zig\README.md
// Line: 873
// Language: zig
// Block ID: dc413ece
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// mylib.zig
const std = @import("std");

// 导出函数给 C 使用
export fn zig_add(a: i32, b: i32) i32 {
    return a + b;
}

// 导出结构体
export const ZigPoint = extern struct {
    x: f64,
    y: f64,
};

export fn zig_distance(p: *const ZigPoint) f64 {
    return std.math.sqrt(p.x * p.x + p.y * p.y);
}
