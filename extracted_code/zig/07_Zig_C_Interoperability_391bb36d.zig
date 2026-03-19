//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 401
// Language: zig
// Block ID: 391bb36d
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// math.zig
const std = @import("std");

// 导出函数供C调用
export fn zig_add(a: c_int, b: c_int) c_int {
    return a + b;
}

// 导出带错误处理的函数
export fn zig_divide(a: c_int, b: c_int, result: *c_int) c_int {
    if (b == 0) return -1;  // 错误码
    result.* = @divTrunc(a, b);
    return 0;  // 成功
}

// 导出结构体操作
export fn create_vector(x: f32, y: f32, z: f32) Vector {
    return Vector{ .x = x, .y = y, .z = z };
}

const Vector = extern struct {
    x: f32,
    y: f32,
    z: f32,
};

export fn vector_length(v: Vector) f32 {
    return std.math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
