/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\01_H264_Decoding.md
 * Line: 379
 * Language: c
 * Block ID: 0faab44f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 去块滤波减少块效应

typedef struct {
    uint8_t alpha;
    uint8_t beta;
    uint8_t tc0[3];  // 色度阈值
} DeblockParams;

// 边界强度计算
int get_boundary_strength(Macroblock *mb_curr, Macroblock *mb_nei,
                          int edge, int is_vertical) {
    // 如果任一块是帧内编码，强度为4
    if (mb_curr->mb_type == I_4x4 || mb_curr->mb_type == I_16x16 ||
        mb_nei->mb_type == I_4x4 || mb_nei->mb_type == I_16x16) {
        return 4;
    }

    // 如果有非零系数，强度为2
    if (has_nonzero_coeffs(mb_curr, edge) || has_nonzero_coeffs(mb_nei, edge)) {
        return 2;
    }

    // 如果参考帧或运动矢量不同，强度为1
    if (mb_curr->ref_idx[0][0] != mb_nei->ref_idx[0][0] ||
        abs(mb_curr->mvd[0][0][0] - mb_nei->mvd[0][0][0]) >= 4 ||
        abs(mb_curr->mvd[0][0][1] - mb_nei->mvd[0][0][1]) >= 4) {
        return 1;
    }

    return 0;
}

// 4像素线滤波
void deblock_luma_4line(uint8_t *p, uint8_t *q, int stride,
                        int alpha, int beta, int tc0) {
    int ap = abs(p[0] - p[-stride * 2]);  // 梯度检查
    int aq = abs(q[0] - q[stride * 2]);

    // 条件滤波
    if (ap < beta && aq < beta && abs(p[0] - q[0]) < alpha) {
        // 计算滤波值
        int delta = clip3((q[0] - p[0] + 4) >> 3, -tc0, tc0);

        p[0] = clip1(p[0] + delta);
        q[0] = clip1(q[0] - delta);

        // 可选的强滤波
        if (ap < ((beta + 1) >> 2)) {
            p[-stride] = p[-stride] + clip3((p[-stride * 2] + p[0] + 1) >> 1 - p[-stride], -1, 1);
        }
        if (aq < ((beta + 1) >> 2)) {
            q[stride] = q[stride] + clip3((q[stride * 2] + q[0] + 1) >> 1 - q[stride], -1, 1);
        }
    }
}

static inline int clip3(int x, int min, int max) {
    return x < min ? min : (x > max ? max : x);
}

static inline int clip1(int x) {
    return clip3(x, 0, 255);
}
