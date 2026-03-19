/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\02_Optical_Flow.md
 * Line: 438
 * Language: c
 * Block ID: 0c0fa282
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 前向后向跟踪验证 */
bool fb_consistency_check(const uint8_t *img_t, const uint8_t *img_t1,
                          int w, int h, Point2f *pt, float fb_threshold) {
    Point2f forward_flow, backward_flow;

    /* 前向：t -> t+1 */
    if (!pyramidal_lk_track(img_t, img_t1, w, h, pt, &forward_flow)) {
        return false;
    }

    /* 后向：t+1 -> t */
    Point2f pt_fwd = {pt->x + forward_flow.x, pt->y + forward_flow.y};
    if (!pyramidal_lk_track(img_t1, img_t, w, h, &pt_fwd, &backward_flow)) {
        return false;
    }

    /* 计算FB误差 */
    float fb_error = hypotf(forward_flow.x + backward_flow.x,
                           forward_flow.y + backward_flow.y);

    return fb_error < fb_threshold;
}

/* NCC（归一化互相关）验证 */
float ncc_score(const uint8_t *img1, const uint8_t *img2,
                int w, int h, int cx, int cy, int win_size) {
    int half = win_size / 2;
    float sum1 = 0, sum2 = 0, sum1_sq = 0, sum2_sq = 0, sum_prod = 0;
    int n = 0;

    for (int dy = -half; dy <= half; dy++) {
        for (int dx = -half; dx <= half; dx++) {
            int x1 = cx + dx, y1 = cy + dy;
            int x2 = cx + dx, y2 = cy + dy;  /* 简化：假设已对齐 */

            if (x1 < 0 || x1 >= w || y1 < 0 || y1 >= h) continue;

            float v1 = img1[y1 * w + x1];
            float v2 = img2[y2 * w + x2];

            sum1 += v1;
            sum2 += v2;
            sum1_sq += v1 * v1;
            sum2_sq += v2 * v2;
            sum_prod += v1 * v2;
            n++;
        }
    }

    float mean1 = sum1 / n;
    float mean2 = sum2 / n;

    float var1 = sum1_sq / n - mean1 * mean1;
    float var2 = sum2_sq / n - mean2 * mean2;
    float cov = sum_prod / n - mean1 * mean2;

    return cov / sqrtf(var1 * var2 + 1e-10);
}
