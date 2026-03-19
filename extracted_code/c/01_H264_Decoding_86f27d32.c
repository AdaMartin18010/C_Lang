/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\01_H264_Decoding.md
 * Line: 201
 * Language: c
 * Block ID: 86f27d32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 宏块结构 (16x16像素)
#define MB_SIZE 16
#define BLOCK_SIZE 4

typedef struct {
    int mb_type;
    int transform_size_8x8_flag;

    // 帧内预测
    int intra4x4_pred_mode[16];   // 4x4块预测模式
    int intra16x16_pred_mode;     // 16x16预测模式
    int chroma_pred_mode;         // 色度预测模式

    // 帧间预测
    int ref_idx[2][4];            // 参考帧索引
    int mvd[2][4][2];             // 运动矢量差

    // 残差
    int16_t residual[16][16];     // 亮度残差
    int16_t residual_cb[8][8];    // Cb残差
    int16_t residual_cr[8][8];    // Cr残差
} Macroblock;

// 帧内4x4预测模式
#define INTRA4X4_VERTICAL   0
#define INTRA4X4_HORIZONTAL 1
#define INTRA4X4_DC         2
#define INTRA4X4_DIAG_DOWN_LEFT 3
#define INTRA4X4_DIAG_DOWN_RIGHT 4
// ... 其他模式

// 帧内4x4垂直预测
void intra4x4_vertical_predict(uint8_t *dst, int stride, uint8_t *top) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            dst[y * stride + x] = top[x];
        }
    }
}

// 帧内4x4 DC预测
void intra4x4_dc_predict(uint8_t *dst, int stride,
                          uint8_t *top, uint8_t *left) {
    int sum = 0;
    for (int i = 0; i < 4; i++) sum += top[i];
    for (int i = 0; i < 4; i++) sum += left[i];
    uint8_t dc = (sum + 4) >> 3;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            dst[y * stride + x] = dc;
        }
    }
}

// 反4x4 DCT变换
void idct4x4(int16_t *coeff, int16_t *block) {
    int16_t tmp[4][4];

    // 水平反变换
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += coeff[i * 4 + k] * dequant_matrix[k][j];
            }
            tmp[i][j] = (sum + 32) >> 6;
        }
    }

    // 垂直反变换
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int sum = 0;
            for (int k = 0; k < 4; k++) {
                sum += tmp[k][j] * dequant_matrix[i][k];
            }
            block[i * 4 + j] = (sum + 32) >> 6;
        }
    }
}
