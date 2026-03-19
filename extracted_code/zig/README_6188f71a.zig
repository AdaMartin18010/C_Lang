//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 282
// Language: zig
// Block ID: 6188f71a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const std = @import("std");

/// SSE 128-bit 向量
pub const Vec4f = @Vector(4, f32);

/// AVX 256-bit 向量
pub const Vec8f = @Vector(8, f32);

/// 确保正确的寄存器传递
export fn simd_operation(a: Vec4f, b: Vec4f) Vec4f {
    // x86_64 System-V: a 在 %xmm0, b 在 %xmm1
    // 返回通过 %xmm0
    return a + b;
}

/// 混合标量和向量参数
export fn mixed_simd(scalar: f64, v1: Vec4f, v2: Vec4f) f64 {
    // System-V AMD64:
    // scalar -> %xmm0
    // v1 -> %xmm1 (需要检查实际分配)
    // v2 -> %xmm2
    const sum = @reduce(.Add, v1 * v2);
    return scalar + sum;
}
