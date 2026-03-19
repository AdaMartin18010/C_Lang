//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 456
// Language: zig
// Block ID: 2cbb26a2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用 Zig 的向量类型

// 向量加法
pub fn vectorAdd(a: []const f32, b: []const f32, result: []f32) void {
    const V = @Vector(4, f32);

    const len = a.len;
    var i: usize = 0;

    // SIMD 处理
    while (i + 4 <= len) : (i += 4) {
        const va: V = a[i..][0..4].*;
        const vb: V = b[i..][0..4].*;
        const vr = va + vb;
        result[i..][0..4].* = vr;
    }

    // 处理剩余
    while (i < len) : (i += 1) {
        result[i] = a[i] + b[i];
    }
}

// 点积 (SIMD)
pub fn dotProductSimd(a: []const f32, b: []const f32) f32 {
    const V = @Vector(8, f32);

    var sum_vec: V = @splat(0);

    const len = a.len;
    var i: usize = 0;

    while (i + 8 <= len) : (i += 8) {
        const va: V = a[i..][0..8].*;
        const vb: V = b[i..][0..8].*;
        sum_vec += va * vb;
    }

    // 水平求和
    var sum = sum_vec[0] + sum_vec[1] + sum_vec[2] + sum_vec[3] +
              sum_vec[4] + sum_vec[5] + sum_vec[6] + sum_vec[7];

    // 剩余元素
    while (i < len) : (i += 1) {
        sum += a[i] * b[i];
    }

    return sum;
}
