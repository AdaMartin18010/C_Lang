/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\02_Optical_Flow.md
 * Line: 144
 * Language: c
 * Block ID: 2f0e9930
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 求解2x2线性系统 Ax = b
 * 使用克莱默法则直接求解
 */
static inline bool solve_2x2(float A00, float A01, float A10, float A11,
                             float b0, float b1, float *x, float *y) {
    float det = A00 * A11 - A01 * A10;

    /* 检查可解性 - 特征值条件 */
    if (fabsf(det) < OF_MIN_EIGENVALUE) {
        return false;  /* 孔径问题或纹理不足 */
    }

    float inv_det = 1.0f / det;
    *x = inv_det * (A11 * b0 - A01 * b1);
    *y = inv_det * (-A10 * b0 + A00 * b1);

    return true;
}

/* Lucas-Kanade单点跟踪 */
bool lucas_kanade_track(const uint8_t *prev_img, const uint8_t *curr_img,
                        int w, int h, Point2f *point, Point2f *flow) {
    /* 预计算梯度 */
    float *Ix = malloc(w * h * sizeof(float));
    float *Iy = malloc(w * h * sizeof(float));
    float *It = malloc(w * h * sizeof(float));

    compute_gradients(prev_img, w, h, Ix, Iy);
    compute_temporal_gradient(prev_img, curr_img, w, h, It);

    int half_win = OF_WINDOW_SIZE / 2;
    int px = (int)(point->x + 0.5f);
    int py = (int)(point->y + 0.5f);

    /* 边界检查 */
    if (px < half_win || px >= w - half_win ||
        py < half_win || py >= h - half_win) {
        free(Ix); free(Iy); free(It);
        return false;
    }

    /* 构建法方程 */
    float A[2][2] = {{0, 0}, {0, 0}};
    float b[2] = {0, 0};

    for (int dy = -half_win; dy <= half_win; dy++) {
        for (int dx = -half_win; dx <= half_win; dx++) {
            int x = px + dx;
            int y = py + dy;
            int idx = y * w + x;

            float ix = Ix[idx];
            float iy = Iy[idx];
            float it = It[idx];

            /* 高斯权重 */
            float weight = expf(-(dx*dx + dy*dy) / (2.0f * half_win * half_win));

            A[0][0] += ix * ix * weight;
            A[0][1] += ix * iy * weight;
            A[1][0] += ix * iy * weight;
            A[1][1] += iy * iy * weight;
            b[0] += -ix * it * weight;
            b[1] += -iy * it * weight;
        }
    }

    bool success = solve_2x2(A[0][0], A[0][1], A[1][0], A[1][1],
                            b[0], b[1], &flow->x, &flow->y);

    free(Ix); free(Iy); free(It);
    return success;
}
