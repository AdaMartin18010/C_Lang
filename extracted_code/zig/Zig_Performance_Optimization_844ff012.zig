//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 769
// Language: zig
// Block ID: 844ff012
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Cooley-Tukey FFT 算法
const std = @import("std");
const math = std.math;

pub fn fft(
    allocator: std.mem.Allocator,
    input: []const f32,
    output_re: []f32,
    output_im: []f32,
) !void {
    const n = input.len;
    std.debug.assert(@popCount(n) == 1);  // 必须是 2 的幂

    // 位反转重排
    for (0..n) |i| {
        const j = bitReverse(i, @intCast(math.log2(n)));
        output_re[j] = input[i];
        output_im[j] = 0;
    }

    // FFT 主循环
    var stage: usize = 2;
    while (stage <= n) : (stage *= 2) {
        const half_stage = stage / 2;

        var i: usize = 0;
        while (i < n) : (i += stage) {
            var j: usize = 0;
            while (j < half_stage) : (j += 1) {
                const angle = -2 * math.pi * @as(f32, @floatFromInt(j)) / @as(f32, @floatFromInt(stage));
                const twiddle_re = math.cos(angle);
                const twiddle_im = math.sin(angle);

                const even_re = output_re[i + j];
                const even_im = output_im[i + j];
                const odd_re = output_re[i + j + half_stage];
                const odd_im = output_im[i + j + half_stage];

                const temp_re = twiddle_re * odd_re - twiddle_im * odd_im;
                const temp_im = twiddle_re * odd_im + twiddle_im * odd_re;

                output_re[i + j] = even_re + temp_re;
                output_im[i + j] = even_im + temp_im;
                output_re[i + j + half_stage] = even_re - temp_re;
                output_im[i + j + half_stage] = even_im - temp_im;
            }
        }
    }

    _ = allocator;
}

fn bitReverse(x: usize, bits: u6) usize {
    var result: usize = 0;
    var i: u6 = 0;
    while (i < bits) : (i += 1) {
        result = (result << 1) | ((x >> i) & 1);
    }
    return result;
}
