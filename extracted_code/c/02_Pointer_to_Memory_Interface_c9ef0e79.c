/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 754
 * Language: c
 * Block ID: c9ef0e79
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 2D卷积的滑动窗口访问
#define WIDTH 1920
#define HEIGHT 1080
#define WINDOW 3

void sliding_window_conv(
    int *input,     // 线性存储的图像
    int *output,
    int kernel[WINDOW][WINDOW]
) {
    #pragma HLS INTERFACE mode=m_axi port=input
    #pragma HLS INTERFACE mode=m_axi port=output

    // 行缓冲区 - 存储多行数据
    int line_buf[WINDOW-1][WIDTH];
    #pragma HLS ARRAY_PARTITION variable=line_buf complete dim=1

    // 窗口缓冲区
    int window[WINDOW][WINDOW];
    #pragma HLS ARRAY_PARTITION variable=window complete

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            #pragma HLS PIPELINE

            // 读取新像素
            int pixel = input[y * WIDTH + x];

            // 更新窗口（移位）
            for (int wy = 0; wy < WINDOW; wy++) {
                for (int wx = 0; wx < WINDOW-1; wx++) {
                    #pragma HLS UNROLL
                    window[wy][wx] = window[wy][wx + 1];
                }
            }

            // 从行缓冲区读取或新像素
            window[0][WINDOW-1] = (y >= WINDOW-1) ? line_buf[0][x] : 0;
            window[1][WINDOW-1] = (y >= 1) ? line_buf[1][x] : pixel;
            window[2][WINDOW-1] = pixel;

            // 更新行缓冲区
            if (y >= 1) line_buf[0][x] = line_buf[1][x];
            line_buf[1][x] = pixel;

            // 计算卷积（边界外为0）
            if (y >= WINDOW-1 && x >= WINDOW-1) {
                int sum = 0;
                for (int wy = 0; wy < WINDOW; wy++) {
                    for (int wx = 0; wx < WINDOW; wx++) {
                        #pragma HLS UNROLL
                        sum += window[wy][wx] * kernel[wy][wx];
                    }
                }
                output[(y-1) * WIDTH + (x-1)] = sum;
            }
        }
    }
}
