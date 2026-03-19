//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 510
// Language: zig
// Block ID: fe7a65d4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 矩阵乘法 (块矩阵 + SIMD)
pub fn matrixMultiply(
    a: []const f32,
    b: []const f32,
    c: []f32,
    n: usize,
) void {
    const BLOCK_SIZE = 64;
    const V = @Vector(4, f32);

    @memset(c, 0);

    var i: usize = 0;
    while (i < n) : (i += BLOCK_SIZE) {
        var j: usize = 0;
        while (j < n) : (j += BLOCK_SIZE) {
            var k: usize = 0;
            while (k < n) : (k += BLOCK_SIZE) {
                // 块矩阵乘法
                multiplyBlock(a, b, c, i, j, k, n, BLOCK_SIZE);
            }
        }
    }
}

fn multiplyBlock(
    a: []const f32,
    b: []const f32,
    c: []f32,
    i0: usize,
    j0: usize,
    k0: usize,
    n: usize,
    block_size: usize,
) void {
    const V = @Vector(4, f32);

    const i_max = @min(i0 + block_size, n);
    const j_max = @min(j0 + block_size, n);
    const k_max = @min(k0 + block_size, n);

    var i = i0;
    while (i < i_max) : (i += 1) {
        var j = j0;
        while (j + 4 <= j_max) : (j += 4) {
            var sum: V = @splat(0);

            var k = k0;
            while (k < k_max) : (k += 1) {
                const a_val = a[i * n + k];
                const b_vec: V = b[k * n + j ..][0..4].*;
                sum += @as(V, @splat(a_val)) * b_vec;
            }

            c[i * n + j ..][0..4].* += sum;
        }
    }
}
