//
// Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
// Line: 175
// Language: zig
// Block ID: d89dbc9e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// mathlib.zig
const std = @import("std");

// 导出C兼容的函数
export fn zig_add(a: i32, b: i32) i32 {
    return a + b;
}

export fn zig_sum(arr: [*]const i32, len: usize) i32 {
    var sum: i32 = 0;
    for (0..len) |i| {
        sum += arr[i];
    }
    return sum;
}

// 复杂结构体
const Point = extern struct {
    x: f64,
    y: f64,
};

export fn zig_distance(p1: Point, p2: Point) f64 {
    const dx = p2.x - p1.x;
    const dy = p2.y - p1.y;
    return std.math.sqrt(dx * dx + dy * dy);
}

// 错误码包装
export fn zig_divide(a: i32, b: i32, result: *i32) i32 {
    if (b == 0) return -1;  // 错误码
    result.* = @divTrunc(a, b);
    return 0;  // 成功
}
