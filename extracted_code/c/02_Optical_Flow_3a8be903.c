/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\02_Optical_Flow.md
 * Line: 293
 * Language: c
 * Block ID: 3a8be903
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 金字塔LK跟踪 - 由粗到精 */
bool pyramidal_lk_track(const ImagePyramid *prev_pyr,
                        const ImagePyramid *curr_pyr,
                        Point2f *point, Point2f *flow) {
    Point2f residual = {0, 0};
    int levels = prev_pyr->num_levels;

    /* 从顶层（粗分辨率）开始 */
    for (int L = levels - 1; L >= 0; L--) {
        /* 缩放因子 */
        float scale = 1.0f / (1 << L);

        /* 当前层级的点位置 */
        Point2f pt = {
            point->x * scale,
            point->y * scale
        };

        /* 加上累积的残差（也缩放到当前层级） */
        pt.x += residual.x * scale;
        pt.y += residual.y * scale;

        /* 在当前层计算光流增量 */
        Point2f delta;
        if (!lucas_kanade_track(prev_pyr->levels[L], curr_pyr->levels[L],
                                prev_pyr->widths[L], prev_pyr->heights[L],
                                &pt, &delta)) {
            return false;
        }

        /* 累积残差 */
        residual.x += delta.x / scale;
        residual.y += delta.y / scale;
    }

    flow->x = residual.x;
    flow->y = residual.y;
    return true;
}
