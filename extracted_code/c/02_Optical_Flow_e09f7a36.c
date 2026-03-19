/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\02_Optical_Flow.md
 * Line: 226
 * Language: c
 * Block ID: e09f7a36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 图像金字塔层级 */
typedef struct {
    uint8_t **levels;   /* 各层级图像 */
    int *widths;        /* 各层级宽度 */
    int *heights;       /* 各层级高度 */
    int num_levels;
} ImagePyramid;

/* 高斯金字塔下采样 */
void pyr_down(const uint8_t *src, int src_w, int src_h,
              uint8_t *dst, int dst_w, int dst_h) {
    /* 5x5高斯核 */
    const float kernel[5] = {0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f};

    for (int y = 0; y < dst_h; y++) {
        for (int x = 0; x < dst_w; x++) {
            float sum = 0;
            int sy = 2 * y;
            int sx = 2 * x;

            for (int ky = -2; ky <= 2; ky++) {
                for (int kx = -2; kx <= 2; kx++) {
                    int iy = fminf(fmaxf(sy + ky, 0), src_h - 1);
                    int ix = fminf(fmaxf(sx + kx, 0), src_w - 1);
                    sum += src[iy * src_w + ix] * kernel[ky + 2] * kernel[kx + 2];
                }
            }
            dst[y * dst_w + x] = (uint8_t)(sum + 0.5f);
        }
    }
}

/* 构建金字塔 */
ImagePyramid* build_pyramid(const uint8_t *img, int w, int h, int levels) {
    ImagePyramid *pyr = malloc(sizeof(ImagePyramid));
    pyr->num_levels = levels;
    pyr->levels = malloc(levels * sizeof(uint8_t*));
    pyr->widths = malloc(levels * sizeof(int));
    pyr->heights = malloc(levels * sizeof(int));

    /* 第0层为原图 */
    pyr->levels[0] = malloc(w * h);
    memcpy(pyr->levels[0], img, w * h);
    pyr->widths[0] = w;
    pyr->heights[0] = h;

    /* 构建下层金字塔 */
    for (int i = 1; i < levels; i++) {
        int prev_w = pyr->widths[i-1];
        int prev_h = pyr->heights[i-1];
        int curr_w = prev_w / 2;
        int curr_h = prev_h / 2;

        pyr->levels[i] = malloc(curr_w * curr_h);
        pyr_down(pyr->levels[i-1], prev_w, prev_h,
                 pyr->levels[i], curr_w, curr_h);
        pyr->widths[i] = curr_w;
        pyr->heights[i] = curr_h;
    }

    return pyr;
}
