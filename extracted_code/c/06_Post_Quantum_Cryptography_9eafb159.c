/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 502
 * Language: c
 * Block ID: 9eafb159
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 格密码学基础概念和数学结构
 * 基于Module-LWE问题
 */

#include <stdint.h>
#include <stddef.h>

/* 环定义 Z_q[X]/(X^n + 1) */
#define ML_KEM_N        256     /* 多项式次数 */
#define ML_KEM_Q        3329    /* 模数 */
#define ML_KEM_ETA1     3       /* 秘密分布参数 */
#define ML_KEM_ETA2     2       /* 错误分布参数 */

/* 多项式类型 */
typedef struct {
    int16_t coeffs[ML_KEM_N];   /* 系数数组 */
} poly;

/* 多项式向量类型 */
typedef struct {
    poly vec[4];                /* 向量维度 k=4 (ML-KEM-1024) */
} polyvec;

/* 模约简：计算 a mod q，结果在 [0, q-1] */
static inline int16_t barrett_reduce(int32_t a) {
    /* Barrett约简：a - floor(a * 2^26 / q) * q / 2^26 */
    const int32_t v = ((1U << 26) + ML_KEM_Q / 2) / ML_KEM_Q;
    int32_t t = (int64_t)v * a >> 26;
    t *= ML_KEM_Q;
    return a - t;
}

/* 模乘：计算 a * b mod q */
static inline int16_t montgomery_reduce(int32_t a) {
    const int32_t qinv = 62209;  /* q^-1 mod 2^16 */
    int32_t t = (int16_t)a * qinv;
    t = (a - (int32_t)(int16_t)t * ML_KEM_Q) >> 16;
    return t;
}

/* NTT（数论变换）：多项式乘法加速 */
void ntt(poly *p) {
    /* 蝶形运算实现 */
    unsigned int len, start, j;
    int16_t t, zeta;

    static const int16_t zetas[128] = {
        /* NTT根表 */
        2285, 2571, 2970, 1812, 1493, 1422, 287, 202,
        705, 1908, 2312, 2077, 1260, 1960, 1762, 390,
        /* ... 更多值 ... */
    };

    for (len = 128; len >= 2; len >>= 1) {
        for (start = 0; start < ML_KEM_N; start = j + len) {
            zeta = zetas[128 - len + start / len];
            for (j = start; j < start + len; ++j) {
                t = montgomery_reduce((int32_t)zeta * p->coeffs[j + len]);
                p->coeffs[j + len] = p->coeffs[j] - t;
                p->coeffs[j] = p->coeffs[j] + t;
            }
        }
    }
}

/* 多项式乘法：使用NTT */
void poly_mul(const poly *a, const poly *b, poly *r) {
    poly at = *a;
    poly bt = *b;

    /* 转NTT域 */
    ntt(&at);
    ntt(&bt);

    /* 逐点乘法 */
    for (int i = 0; i < ML_KEM_N; i++) {
        r->coeffs[i] = montgomery_reduce((int32_t)at.coeffs[i] * bt.coeffs[i]);
    }

    /* 逆NTT */
    inv_ntt(r);
}

/* 采样中心二项分布 */
void poly_getnoise(poly *r, const uint8_t *seed, uint8_t nonce) {
    /* 使用PRF生成随机数 */
    uint8_t buf[ML_KEM_ETA1 * ML_KEM_N / 4];
    prf(buf, sizeof(buf), seed, nonce);

    /* 转换为中心二项分布 */
    for (int i = 0; i < ML_KEM_N / 4; i++) {
        uint8_t t = buf[i];
        int16_t d = 0;
        for (int j = 0; j < 4; j++) {
            d += (t >> j) & 1;
            d -= (t >> (j + 4)) & 1;
        }
        r->coeffs[4 * i + 0] = d;
    }
}

/* 压缩/解压缩（用于密钥封装） */
void poly_compress(const poly *a, uint8_t *r, int d) {
    /* 将每个系数从16位压缩到d位 */
    unsigned int i, j;
    uint32_t t[8];

    for (i = 0; i < ML_KEM_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            t[j] = (((uint32_t)a->coeffs[8 * i + j] << d) + ML_KEM_Q / 2) / ML_KEM_Q;
            t[j] &= ((uint32_t)1 << d) - 1;
        }

        /* 打包 */
        r[0] = t[0] | (t[1] << d);
        r[1] = (t[1] >> (8 - d)) | (t[2] << (2 * d - 8)) | (t[3] << (3 * d - 8));
        /* ... */
        r += d;
    }
}
