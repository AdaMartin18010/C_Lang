/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\02_Optical_Flow.md
 * Line: 341
 * Language: c
 * Block ID: f05caab2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 双线性插值获取像素值 */
static inline float bilinear_interp(const uint8_t *img, int w, int h,
                                    float x, float y) {
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = fminf(x0 + 1, w - 1);
    int y1 = fminf(y0 + 1, h - 1);

    float fx = x - x0;
    float fy = y - y0;

    float v00 = img[y0 * w + x0];
    float v01 = img[y0 * w + x1];
    float v10 = img[y1 * w + x0];
    float v11 = img[y1 * w + x1];

    return (1 - fx) * (1 - fy) * v00 +
           fx * (1 - fy) * v01 +
           (1 - fx) * fy * v10 +
           fx * fy * v11;
}

/* 迭代求精 - 亚像素级精度 */
bool lk_track_iterative(const uint8_t *prev_img, const uint8_t *curr_img,
                        int w, int h, Point2f *point, Point2f *flow,
                        int max_iter) {
    float g[2] = {0, 0};  /* 当前估计的位移 */

    for (int iter = 0; iter < max_iter; iter++) {
        /* 计算在当前估计位置的残差 */
        float A[2][2] = {{0, 0}, {0, 0}};
        float b[2] = {0, 0};

        int half_win = OF_WINDOW_SIZE / 2;
        int px = (int)point->x;
        int py = (int)point->y;

        for (int dy = -half_win; dy <= half_win; dy++) {
            for (int dx = -half_win; dx <= half_win; dx++) {
                float x = px + dx;
                float y = py + dy;

                /* 前一帧位置 */
                float prev_val = bilinear_interp(prev_img, w, h, x, y);

                /* 当前帧对应位置（加上当前估计） */
                float curr_val = bilinear_interp(curr_img, w, h,
                                                 x + g[0], y + g[1]);

                /* 计算梯度（使用中心差分） */
                float ix = (bilinear_interp(prev_img, w, h, x + 1, y) -
                           bilinear_interp(prev_img, w, h, x - 1, y)) * 0.5f;
                float iy = (bilinear_interp(prev_img, w, h, x, y + 1) -
                           bilinear_interp(prev_img, w, h, x, y - 1)) * 0.5f;

                float it = curr_val - prev_val;

                /* 累积 */
                A[0][0] += ix * ix;
                A[0][1] += ix * iy;
                A[1][1] += iy * iy;
                b[0] += -ix * it;
                b[1] += -iy * it;
            }
        }
        A[1][0] = A[0][1];

        /* 求解增量 */
        float eta[2];
        if (!solve_2x2(A[0][0], A[0][1], A[1][0], A[1][1],
                      b[0], b[1], &eta[0], &eta[1])) {
            return false;
        }

        /* 更新估计 */
        g[0] += eta[0];
        g[1] += eta[1];

        /* 收敛检查 */
        if (eta[0] * eta[0] + eta[1] * eta[1] < OF_EPSILON) {
            break;
        }
    }

    flow->x = g[0];
    flow->y = g[1];
    return true;
}
