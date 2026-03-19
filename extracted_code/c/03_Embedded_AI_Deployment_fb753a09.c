/*
 * Auto-generated from: 11_Machine_Learning_C\03_Embedded_AI_Deployment.md
 * Line: 611
 * Language: c
 * Block ID: fb753a09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file int8_quantization.c
 * @brief INT8对称量化完整实现
 *
 * 对称量化公式:
 *   q = round(r / scale)
 *   r = q * scale
 *
 * scale计算:
 *   scale = max(abs(rmin), abs(rmax)) / 127
 */

#include "quantization_types.h"
#include <string.h>

/* ========== 量化工具函数 ========== */

/**
 * @brief 计算对称量化scale
 * @param rmin 最小值
 * @param rmax 最大值
 * @return scale值
 */
float int8_calc_scale(float rmin, float rmax)
{
    float abs_max = fmaxf(fabsf(rmin), fabsf(rmax));

    /* 避免除以零 */
    if (abs_max < 1e-8f) {
        return 1.0f;
    }

    return abs_max / 127.0f;
}

/**
 * @brief FP32 -> INT8 量化
 * @param input 输入FP32数组
 * @param output 输出INT8数组
 * @param size 数组大小
 * @param scale 缩放因子
 */
void quantize_fp32_to_int8(const float* input,
                           int8_t* output,
                           uint32_t size,
                           float scale)
{
    /* 量化公式: q = round(r / scale) */
    const float inv_scale = 1.0f / scale;

    for (uint32_t i = 0; i < size; i++) {
        /* 缩放并四舍五入 */
        float scaled = input[i] * inv_scale;

        /* 限制在INT8范围内 */
        if (scaled > 127.0f) scaled = 127.0f;
        if (scaled < -128.0f) scaled = -128.0f;

        output[i] = (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
    }
}

/**
 * @brief INT8 -> FP32 反量化
 * @param input 输入INT8数组
 * @param output 输出FP32数组
 * @param size 数组大小
 * @param scale 缩放因子
 */
void dequantize_int8_to_fp32(const int8_t* input,
                              float* output,
                              uint32_t size,
                              float scale)
{
    /* 反量化公式: r = q * scale */
    for (uint32_t i = 0; i < size; i++) {
        output[i] = (float)input[i] * scale;
    }
}

/* ========== 量化矩阵乘法 ========== */

/**
 * @brief INT8量化矩阵乘法: C = A * B
 * @param A 左矩阵 (M x K)
 * @param B 右矩阵 (K x N)
 * @param C 结果矩阵 (M x N)
 * @param M 行数
 * @param K 内维
 * @param N 列数
 * @param scale_a A的scale
 * @param scale_b B的scale
 * @param scale_c C的scale
 *
 * 注意: 输出需要反量化后再量化到INT8
 */
void quantized_matmul_int8(const int8_t* A,
                            const int8_t* B,
                            int8_t* C,
                            uint32_t M, uint32_t K, uint32_t N,
                            float scale_a,
                            float scale_b,
                            float scale_c)
{
    /* 累积使用INT32防止溢出 */
    const float combined_scale = (scale_a * scale_b) / scale_c;

    for (uint32_t m = 0; m < M; m++) {
        for (uint32_t n = 0; n < N; n++) {
            int32_t acc = 0;

            /* 内积计算 */
            for (uint32_t k = 0; k < K; k++) {
                acc += (int32_t)A[m * K + k] * (int32_t)B[k * N + n];
            }

            /* 重新量化到INT8 */
            float scaled = (float)acc * combined_scale;

            /* 限制范围并四舍五入 */
            if (scaled > 127.0f) scaled = 127.0f;
            if (scaled < -128.0f) scaled = -128.0f;

            C[m * N + n] = (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
        }
    }
}

/**
 * @brief 优化的4x4 INT8矩阵乘法块
 * 使用局部变量累加，减少内存访问
 */
static void matmul_4x4_int8_block(const int8_t* A, const int8_t* B, int8_t* C,
                                   uint32_t K, float combined_scale)
{
    int32_t acc[4][4] = {{0}};

    /* 展开内层循环 */
    for (uint32_t k = 0; k < K; k++) {
        int8_t a0 = A[0 * K + k];
        int8_t a1 = A[1 * K + k];
        int8_t a2 = A[2 * K + k];
        int8_t a3 = A[3 * K + k];

        int8_t b0 = B[k * 4 + 0];
        int8_t b1 = B[k * 4 + 1];
        int8_t b2 = B[k * 4 + 2];
        int8_t b3 = B[k * 4 + 3];

        acc[0][0] += a0 * b0; acc[0][1] += a0 * b1;
        acc[0][2] += a0 * b2; acc[0][3] += a0 * b3;

        acc[1][0] += a1 * b0; acc[1][1] += a1 * b1;
        acc[1][2] += a1 * b2; acc[1][3] += a1 * b3;

        acc[2][0] += a2 * b0; acc[2][1] += a2 * b1;
        acc[2][2] += a2 * b2; acc[2][3] += a2 * b3;

        acc[3][0] += a3 * b0; acc[3][1] += a3 * b1;
        acc[3][2] += a3 * b2; acc[3][3] += a3 * b3;
    }

    /* 重新量化并存储 */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float scaled = (float)acc[i][j] * combined_scale;
            if (scaled > 127.0f) scaled = 127.0f;
            if (scaled < -128.0f) scaled = -128.0f;
            C[i * 4 + j] = (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
        }
    }
}

/* ========== 量化卷积实现 ========== */

/**
 * @brief INT8量化2D卷积 (简化版，无padding)
 * @param input 输入特征图 [H x W x C_in]
 * @param kernel 卷积核 [K_h x K_w x C_in x C_out]
 * @param output 输出特征图 [H_out x W_out x C_out]
 */
void quantized_conv2d_int8(const int8_t* input,
                            const int8_t* kernel,
                            int8_t* output,
                            uint32_t H, uint32_t W, uint32_t C_in,
                            uint32_t K_h, uint32_t K_w,
                            uint32_t C_out,
                            uint32_t stride_h, uint32_t stride_w,
                            float input_scale,
                            float kernel_scale,
                            float output_scale)
{
    uint32_t H_out = (H - K_h) / stride_h + 1;
    uint32_t W_out = (W - K_w) / stride_w + 1;

    const float combined_scale = (input_scale * kernel_scale) / output_scale;

    for (uint32_t co = 0; co < C_out; co++) {
        for (uint32_t ho = 0; ho < H_out; ho++) {
            for (uint32_t wo = 0; wo < W_out; wo++) {
                int32_t acc = 0;

                /* 卷积窗口 */
                for (uint32_t kh = 0; kh < K_h; kh++) {
                    for (uint32_t kw = 0; kw < K_w; kw++) {
                        uint32_t ih = ho * stride_h + kh;
                        uint32_t iw = wo * stride_w + kw;

                        for (uint32_t ci = 0; ci < C_in; ci++) {
                            int8_t in_val = input[(ih * W + iw) * C_in + ci];
                            int8_t k_val = kernel[((co * K_h + kh) * K_w + kw) * C_in + ci];
                            acc += (int32_t)in_val * (int32_t)k_val;
                        }
                    }
                }

                /* 重新量化 */
                float scaled = (float)acc * combined_scale;
                if (scaled > 127.0f) scaled = 127.0f;
                if (scaled < -128.0f) scaled = -128.0f;

                output[(ho * W_out + wo) * C_out + co] =
                    (int8_t)(scaled > 0 ? (scaled + 0.5f) : (scaled - 0.5f));
            }
        }
    }
}
