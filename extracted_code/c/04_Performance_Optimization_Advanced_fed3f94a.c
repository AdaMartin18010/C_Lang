/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\04_Performance_Optimization_Advanced.md
 * Line: 1618
 * Language: c
 * Block ID: fed3f94a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file neon_optimization.c
 * @brief ARM NEON SIMD优化指南
 *
 * NEON是ARM架构的SIMD扩展，支持128位向量操作
 * 适用于移动设备、嵌入式系统和ARM服务器
 */

#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* NEON类型说明：
 * float32x4_t: 4个float的向量
 * int32x4_t: 4个int32的向量
 * uint8x16_t: 16个uint8的向量
 */

#ifdef __ARM_NEON

/**
 * @brief NEON向量加法
 */
void add_neon(float *dst, const float *a, const float *b, int n) {
    int i = 0;
    /* 每次处理4个float（128位） */
    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);  /* 加载4个float */
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vr = vaddq_f32(va, vb); /* 向量加法 */
        vst1q_f32(&dst[i], vr);              /* 存储结果 */
    }
    /* 剩余元素 */
    for (; i < n; i++) {
        dst[i] = a[i] + b[i];
    }
}

/**
 * @brief NEON点积 - 使用乘累加
 */
float dot_neon(const float *a, const float *b, int n) {
    float32x4_t sum4 = vdupq_n_f32(0.0f);  /* 初始化为0 */
    int i = 0;

    for (; i <= n - 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        /* 乘累加: sum += a * b */
        sum4 = vfmaq_f32(sum4, va, vb);
    }

    /* 水平求和 */
    float32x2_t sum2 = vadd_f32(vget_low_f32(sum4), vget_high_f32(sum4));
    float32x2_t sum1 = vpadd_f32(sum2, sum2);
    float sum = vget_lane_f32(sum1, 0);

    for (; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

/**
 * @brief NEON图像处理 - RGB转灰度
 *
 * 灰度 = 0.299*R + 0.587*G + 0.114*B
 * NEON可同时处理多个像素
 */
void rgb_to_gray_neon(const uint8_t *rgb, uint8_t *gray, int num_pixels) {
    /* 系数向量 */
    uint8x8_t r_coeff = vdup_n_u8(77);   /* 0.299 * 256 */
    uint8x8_t g_coeff = vdup_n_u8(150);  /* 0.587 * 256 */
    uint8x8_t b_coeff = vdup_n_u8(29);   /* 0.114 * 256 */

    int i = 0;
    for (; i <= num_pixels - 8; i += 8) {
        /* 加载24字节（8个RGB像素） */
        uint8x8x3_t rgb_vec = vld3_u8(&rgb[i * 3]);

        /* 分别提取R, G, B通道 */
        uint8x8_t r = rgb_vec.val[0];
        uint8x8_t g = rgb_vec.val[1];
        uint8x8_t b = rgb_vec.val[2];

        /* 计算加权和 */
        uint16x8_t sum = vmull_u8(r, r_coeff);
        sum = vmlal_u8(sum, g, g_coeff);
        sum = vmlal_u8(sum, b, b_coeff);

        /* 右移8位（相当于除以256）并缩小到8位 */
        uint8x8_t gray_vec = vshrn_n_u16(sum, 8);

        /* 存储8个灰度值 */
        vst1_u8(&gray[i], gray_vec);
    }

    /* 剩余像素 */
    for (; i < num_pixels; i++) {
        gray[i] = (77 * rgb[i*3] + 150 * rgb[i*3+1] + 29 * rgb[i*3+2]) >> 8;
    }
}

/**
 * @brief NEON内存拷贝 - 使用宽向量加载/存储
 */
void memcpy_neon(void *dst, const void *src, size_t n) {
    uint8_t *d = dst;
    const uint8_t *s = src;

    size_t i = 0;
    /* 每次拷贝64字节（4个128位寄存器） */
    for (; i <= n - 64; i += 64) {
        uint8x16_t v0 = vld1q_u8(&s[i]);
        uint8x16_t v1 = vld1q_u8(&s[i + 16]);
        uint8x16_t v2 = vld1q_u8(&s[i + 32]);
        uint8x16_t v3 = vld1q_u8(&s[i + 48]);

        vst1q_u8(&d[i], v0);
        vst1q_u8(&d[i + 16], v1);
        vst1q_u8(&d[i + 32], v2);
        vst1q_u8(&d[i + 48], v3);
    }
    /* 剩余字节 */
    for (; i < n; i++) {
        d[i] = s[i];
    }
}

#endif /* __ARM_NEON */

/* 标量参考实现 */
void rgb_to_gray_scalar(const uint8_t *rgb, uint8_t *gray, int num_pixels) {
    for (int i = 0; i < num_pixels; i++) {
        uint16_t r = rgb[i * 3];
        uint16_t g = rgb[i * 3 + 1];
        uint16_t b = rgb[i * 3 + 2];
        gray[i] = (uint8_t)((77 * r + 150 * g + 29 * b) >> 8);
    }
}

void benchmark_neon(void) {
    printf("=== ARM NEON Benchmark ===\n\n");

#ifdef __ARM_NEON
    printf("NEON is available on this platform\n\n");

    /* RGB转灰度测试 */
    const int NUM_PIXELS = 1920 * 1080;  /* 1080p图像 */
    uint8_t *rgb = malloc(NUM_PIXELS * 3);
    uint8_t *gray_scalar = malloc(NUM_PIXELS);
    uint8_t *gray_neon = malloc(NUM_PIXELS);

    /* 填充测试数据 */
    for (int i = 0; i < NUM_PIXELS * 3; i++) {
        rgb[i] = (uint8_t)(i % 256);
    }

    /* 标量版本 */
    clock_t start = clock();
    rgb_to_gray_scalar(rgb, gray_scalar, NUM_PIXELS);
    clock_t scalar_time = clock() - start;
    printf("RGB->Gray Scalar: %ld ms\n", scalar_time * 1000 / CLOCKS_PER_SEC);

    /* NEON版本 */
    start = clock();
    rgb_to_gray_neon(rgb, gray_neon, NUM_PIXELS);
    clock_t neon_time = clock() - start;
    printf("RGB->Gray NEON:   %ld ms (%.2fx speedup)\n",
           neon_time * 1000 / CLOCKS_PER_SEC,
           (double)scalar_time / neon_time);

    /* 验证结果 */
    int diff = 0;
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (abs((int)gray_scalar[i] - (int)gray_neon[i]) > 1) {
            diff++;
        }
    }
    printf("Differences: %d\n", diff);

    free(rgb);
    free(gray_scalar);
    free(gray_neon);
#else
    printf("NEON not available on this platform (x86/x64)\n");
    printf("This code demonstrates NEON syntax for ARM platforms\n");
#endif
}

int main(void) {
    benchmark_neon();
    return 0;
}
