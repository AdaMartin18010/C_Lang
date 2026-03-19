//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 573
// Language: zig
// Block ID: 7103b2f3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// RGB 到灰度转换 (SIMD)
pub fn rgbToGrayscaleSimd(input: []const u8, output: []u8, pixel_count: usize) void {
    const V = @Vector(16, u8);
    const VW = @Vector(16, u16);

    const r_coeff: u16 = 77;   // 0.299 * 256
    const g_coeff: u16 = 150;  // 0.587 * 256
    const b_coeff: u16 = 29;   // 0.114 * 256

    var i: usize = 0;
    while (i + 48 <= pixel_count * 3) : (i += 48) {
        // 加载 16 个像素的 RGB 数据 (48 字节)
        const rgb_data = input[i..][0..48].*;

        // 分离 R, G, B
        var gray_pixels: [16]u8 = undefined;

        var j: usize = 0;
        while (j < 16) : (j += 1) {
            const r = @as(u16, rgb_data[j * 3]);
            const g = @as(u16, rgb_data[j * 3 + 1]);
            const b = @as(u16, rgb_data[j * 3 + 2]);

            const gray = (r * r_coeff + g * g_coeff + b * b_coeff) >> 8;
            gray_pixels[j] = @truncate(gray);
        }

        @memcpy(output[i / 3 ..][0..16], &gray_pixels);
    }
}
