/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 1127
 * Language: c
 * Block ID: d8f78a0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 双线性插值图像缩放
 * 输入：源图像 + 缩放比例
 * 输出：目标图像
 */

#include "ap_fixed.h"

typedef ap_uint<8> pixel_t;
typedef ap_ufixed<16,8> coord_t;  // 8位整数 + 8位小数

void image_scaler(
    pixel_t *src_img,   // 源图像 (AXI主接口)
    pixel_t *dst_img,   // 目标图像
    ap_uint<12> src_w, ap_uint<12> src_h,  // 源尺寸
    ap_uint<12> dst_w, ap_uint<12> dst_h   // 目标尺寸
) {
    #pragma HLS INTERFACE mode=m_axi port=src_img depth=2073600  // 1920x1080
    #pragma HLS INTERFACE mode=m_axi port=dst_img depth=2073600
    #pragma HLS INTERFACE mode=s_axilite port=src_w
    #pragma HLS INTERFACE mode=s_axilite port=src_h
    #pragma HLS INTERFACE mode=s_axilite port=dst_w
    #pragma HLS INTERFACE mode=s_axilite port=dst_h

    // 计算缩放比例
    coord_t x_ratio = (coord_t)src_w / (coord_t)dst_w;
    coord_t y_ratio = (coord_t)src_h / (coord_t)dst_h;

    // 行缓冲区（存储2行源图像）
    pixel_t line_buf[2][1920];
    #pragma HLS ARRAY_PARTITION variable=line_buf complete dim=1

    for (ap_uint<12> y = 0; y < dst_h; y++) {
        coord_t src_y = y * y_ratio;
        ap_uint<12> y0 = src_y;
        ap_uint<12> y1 = (y0 + 1 < src_h) ? y0 + 1 : y0;
        coord_t wy = src_y - y0;

        // 预加载两行
        for (ap_uint<12> x = 0; x < src_w; x++) {
            #pragma HLS PIPELINE
            if (y == 0 || y0 != ((y-1) * y_ratio)) {
                line_buf[0][x] = src_img[y0 * src_w + x];
                line_buf[1][x] = src_img[y1 * src_w + x];
            }
        }

        // 处理当前目标行
        for (ap_uint<12> x = 0; x < dst_w; x++) {
            #pragma HLS PIPELINE II=1

            coord_t src_x = x * x_ratio;
            ap_uint<12> x0 = src_x;
            ap_uint<12> x1 = (x0 + 1 < src_w) ? x0 + 1 : x0;
            coord_t wx = src_x - x0;

            // 读取4个相邻像素
            pixel_t p00 = line_buf[0][x0];
            pixel_t p01 = line_buf[0][x1];
            pixel_t p10 = line_buf[1][x0];
            pixel_t p11 = line_buf[1][x1];

            // 双线性插值
            coord_t val = (coord_t)(p00) * (1-wx) * (1-wy) +
                          (coord_t)(p01) * wx * (1-wy) +
                          (coord_t)(p10) * (1-wx) * wy +
                          (coord_t)(p11) * wx * wy;

            dst_img[y * dst_w + x] = (pixel_t)val;
        }
    }
}
