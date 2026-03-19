/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\03_Edge_Detection.md
 * Line: 679
 * Language: c
 * Block ID: b145fde8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* fixed_point_canny.c - 无浮点运算版本，适合嵌入式 */
#define FIXED_SHIFT 8
#define FIXED_SCALE (1 << FIXED_SHIFT)
#define TO_FIXED(x) ((int)((x) * FIXED_SCALE))
#define FROM_FIXED(x) ((x) >> FIXED_SHIFT)

/* 预计算的整数高斯核 (sigma=1.0, 3x3) */
static const int gaussian_kernel_3x3[9] = {
    16, 32, 16,
    32, 64, 32,
    16, 32, 16
}; /* 和为256 = 2^8，便于移位除法 */

static void gaussian_smooth_fixed(const uint8_t *src, int16_t *dst,
                                   int width, int height)
{
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += src[(y+ky)*width + (x+kx)] *
                           gaussian_kernel_3x3[(ky+1)*3 + (kx+1)];
                }
            }
            dst[y*width + x] = sum >> 8;  /* 除以256 */
        }
    }
}
