/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\04_SIMD_Optimization_Case.md
 * Line: 343
 * Language: c
 * Block ID: 63bb8284
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RGB转灰度 (NEON优化)
void rgb_to_gray_neon(const uint8_t* rgb, uint8_t* gray, int width, int height) {
    const int pixels = width * height;
    int i = 0;

    // 系数：Y = 0.299R + 0.587G + 0.114B
    // 使用Q16定点：0.299*65536=19595, 0.587*65536=38470, 0.114*65536=7471
    const uint16x8_t coeff_r = vdupq_n_u16(19595);
    const uint16x8_t coeff_g = vdupq_n_u16(38470);
    const uint16x8_t coeff_b = vdupq_n_u16(7471);

    for (; i + 8 <= pixels; i += 8) {
        // 加载24字节 (8个RGB像素)
        uint8x16x3_t rgb_data = vld3q_u8(rgb + i * 3);

        // 扩展到低16位
        uint16x8_t r = vmovl_u8(vget_low_u8(rgb_data.val[0]));
        uint16x8_t g = vmovl_u8(vget_low_u8(rgb_data.val[1]));
        uint16x8_t b = vmovl_u8(vget_low_u8(rgb_data.val[2]));

        // 乘法并累加 (32位结果)
        uint32x4_t y_low = vmull_u16(vget_low_u16(r), vget_low_u16(coeff_r));
        y_low = vmlal_u16(y_low, vget_low_u16(g), vget_low_u16(coeff_g));
        y_low = vmlal_u16(y_low, vget_low_u16(b), vget_low_u16(coeff_b));

        uint32x4_t y_high = vmull_u16(vget_high_u16(r), vget_high_u16(coeff_r));
        y_high = vmlal_u16(y_high, vget_high_u16(g), vget_high_u16(coeff_g));
        y_high = vmlal_u16(y_high, vget_high_u16(b), vget_high_u16(coeff_b));

        // 右移16位并窄化到8位
        uint16x4_t y_low_narrow = vshrn_n_u32(y_low, 16);
        uint16x4_t y_high_narrow = vshrn_n_u32(y_high, 16);
        uint8x8_t y = vmovn_u16(vcombine_u16(y_low_narrow, y_high_narrow));

        // 存储
        vst1_u8(gray + i, y);
    }

    // 处理剩余像素
    for (; i < pixels; i++) {
        int r = rgb[i*3 + 0];
        int g = rgb[i*3 + 1];
        int b = rgb[i*3 + 2];
        gray[i] = (uint8_t)((19595*r + 38470*g + 7471*b) >> 16);
    }
}
