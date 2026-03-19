//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 441
// Language: zig
// Block ID: 0f847729
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// simd.zig - WebAssembly SIMD

pub fn add_vectors_simd(a: []const f32, b: []const f32, result: []f32) void {
    const V = @Vector(4, f32);

    var i: usize = 0;
    while (i + 4 <= a.len) : (i += 4) {
        const va: V = a[i..][0..4].*;
        const vb: V = b[i..][0..4].*;
        const vr = va + vb;
        result[i..][0..4].* = vr;
    }

    while (i < a.len) : (i += 1) {
        result[i] = a[i] + b[i];
    }
}

pub fn dot_product_simd(a: []const f32, b: []const f32) f32 {
    const V = @Vector(4, f32);

    var sum_vec: V = @splat(0);

    var i: usize = 0;
    while (i + 4 <= a.len) : (i += 4) {
        const va: V = a[i..][0..4].*;
        const vb: V = b[i..][0..4].*;
        sum_vec += va * vb;
    }

    var sum = sum_vec[0] + sum_vec[1] + sum_vec[2] + sum_vec[3];

    while (i < a.len) : (i += 1) {
        sum += a[i] * b[i];
    }

    return sum;
}
