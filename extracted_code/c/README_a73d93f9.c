/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\README.md
 * Line: 915
 * Language: c
 * Block ID: a73d93f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file kyber_demo.c
 * @brief CRYSTALS-Kyber 密钥封装机制概念实现
 *
 * Kyber基于模格(Module-LWE)问题
 * 这是简化版示例，展示核心概念
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Kyber参数（简化版 Kyber-512） */
#define KYBER_N 256           /* 多项式次数 */
#define KYBER_Q 3329          /* 模数 */
#define KYBER_ETA 3           /* 错误分布参数 */
#define KYBER_DU 10           /* 压缩参数 */
#define KYBER_DV 4            /* 压缩参数 */

/* 多项式环 R_q = Z_q[X]/(X^n + 1) */
typedef struct {
    int16_t coeffs[KYBER_N];  /* 系数范围 [0, Q-1] */
} Poly;

/* 多项式向量 */
typedef struct {
    Poly vec[2];  /* 对于 Kyber-512, k=2 */
} PolyVec;

/* Kyber公钥 */
typedef struct {
    PolyVec t;           /* A*s + e */
    uint8_t seed[32];    /* 生成A的种子 */
} KyberPublicKey;

/* Kyber私钥 */
typedef struct {
    PolyVec s;           /* 秘密向量 */
} KyberPrivateKey;

/* 密文 */
typedef struct {
    PolyVec u;           /* A^T*r + e1 */
    Poly v;              /* t^T*r + e2 + m */
} KyberCiphertext;

/* 共享密钥长度 */
#define KYBER_SHARED_SECRET_BYTES 32

/**
 * @brief 中心二项分布采样（模拟）
 *
 * 从中心二项分布采样小整数
 * 实际实现需要安全随机数生成器
 */
int16_t cbd_sample(void) {
    /* 简化：返回 -ETA 到 ETA 之间的随机值 */
    return (rand() % (2 * KYBER_ETA + 1)) - KYBER_ETA;
}

/**
 * @brief 多项式乘法（NTT域）
 *
 * 实际实现使用数论变换(NTT)加速
 * 这里展示简化版本
 */
void poly_mul(const Poly *a, const Poly *b, Poly *result) {
    /* 简化的多项式乘法（不使用NTT） */
    memset(result->coeffs, 0, sizeof(result->coeffs));

    for (int i = 0; i < KYBER_N; i++) {
        for (int j = 0; j < KYBER_N; j++) {
            int idx = (i + j) % KYBER_N;
            int sign = (i + j < KYBER_N) ? 1 : -1;  /* X^n = -1 */
            result->coeffs[idx] += sign * a->coeffs[i] * b->coeffs[j];
            result->coeffs[idx] %= KYBER_Q;
        }
    }
}

/**
 * @brief 多项式加法
 */
void poly_add(const Poly *a, const Poly *b, Poly *result) {
    for (int i = 0; i < KYBER_N; i++) {
        int sum = a->coeffs[i] + b->coeffs[i];
        result->coeffs[i] = ((sum % KYBER_Q) + KYBER_Q) % KYBER_Q;
    }
}

/**
 * @brief 生成小多项式（错误多项式）
 */
void poly_generate_small(Poly *p) {
    for (int i = 0; i < KYBER_N; i++) {
        p->coeffs[i] = cbd_sample();
        /* 映射到 [0, Q-1] */
        p->coeffs[i] = ((p->coeffs[i] % KYBER_Q) + KYBER_Q) % KYBER_Q;
    }
}

/**
 * @brief Kyber密钥生成
 *
 * 算法:
 * 1. 生成随机种子
 * 2. 从种子扩展矩阵A
 * 3. 采样秘密s和错误e
 * 4. 计算 t = A*s + e
 * 5. 公钥 = (t, seed), 私钥 = s
 */
void kyber_keygen(KyberPublicKey *pk, KyberPrivateKey *sk) {
    printf("=== Kyber密钥生成 ===\n");

    /* 1. 生成随机种子（模拟） */
    for (int i = 0; i < 32; i++) {
        pk->seed[i] = rand() & 0xFF;
    }
    printf("1. 生成随机种子\n");

    /* 2. 采样秘密向量 s */
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&sk->s.vec[i]);
    }
    printf("2. 采样秘密向量 s\n");

    /* 3. 采样错误向量 e */
    PolyVec e;
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&e.vec[i]);
    }
    printf("3. 采样错误向量 e\n");

    /* 4. 模拟计算 t = A*s + e */
    /* 实际需要从种子扩展矩阵A */
    printf("4. 计算公钥 t = A·s + e\n");
    for (int i = 0; i < 2; i++) {
        /* 简化的矩阵向量乘法 */
        memcpy(&pk->t.vec[i], &sk->s.vec[i], sizeof(Poly));
        poly_add(&pk->t.vec[i], &e.vec[i], &pk->t.vec[i]);
    }

    printf("密钥生成完成\n\n");
}

/**
 * @brief Kyber加密（封装）
 *
 * 算法:
 * 1. 从公钥恢复矩阵A
 * 2. 采样随机r, e1, e2
 * 3. 计算 u = A^T*r + e1
 * 4. 计算 v = t^T*r + e2 + m
 * 5. 压缩并输出密文(u, v)
 */
void kyber_encaps(const KyberPublicKey *pk,
                  const uint8_t message[32],
                  KyberCiphertext *ct,
                  uint8_t shared_secret[KYBER_SHARED_SECRET_BYTES]) {
    printf("=== Kyber封装 ===\n");

    /* 1. 采样随机向量 r */
    PolyVec r;
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&r.vec[i]);
    }
    printf("1. 采样随机向量 r\n");

    /* 2. 采样错误 e1, e2 */
    PolyVec e1;
    Poly e2;
    for (int i = 0; i < 2; i++) {
        poly_generate_small(&e1.vec[i]);
    }
    poly_generate_small(&e2);
    printf("2. 采样错误多项式\n");

    /* 3. 计算 u = A^T*r + e1 */
    printf("3. 计算 u = A^T·r + e1\n");
    for (int i = 0; i < 2; i++) {
        /* 简化的计算 */
        memcpy(&ct->u.vec[i], &r.vec[i], sizeof(Poly));
        poly_add(&ct->u.vec[i], &e1.vec[i], &ct->u.vec[i]);
    }

    /* 4. 计算 v = t^T*r + e2 + m (简化) */
    printf("4. 计算 v = t^T·r + e2 + Encode(m)\n");
    memcpy(&ct->v, &e2, sizeof(Poly));

    /* 派生共享密钥 */
    printf("5. 派生共享密钥\n");
    for (int i = 0; i < KYBER_SHARED_SECRET_BYTES; i++) {
        shared_secret[i] = rand() & 0xFF;
    }

    printf("封装完成\n\n");
}

/**
 * @brief Kyber解密（解封装）
 *
 * 算法:
 * 1. 计算 m' = v - s^T*u
 * 2. 解码并输出共享密钥
 */
void kyber_decaps(const KyberPrivateKey *sk,
                  const KyberCiphertext *ct,
                  uint8_t shared_secret[KYBER_SHARED_SECRET_BYTES]) {
    printf("=== Kyber解封装 ===\n");

    /* 计算 m' = v - s^T*u */
    printf("1. 计算 m' = v - s^T·u\n");

    /* 解码消息并派生共享密钥 */
    printf("2. 解码消息并派生共享密钥\n");
    for (int i = 0; i < KYBER_SHARED_SECRET_BYTES; i++) {
        shared_secret[i] = rand() & 0xFF;
    }

    printf("解封装完成\n\n");
}

/* 主函数演示 */
int main(void) {
    srand(42);

    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║     CRYSTALS-Kyber 密钥封装机制 (KEM) 演示              ║\n");
    printf("║     (基于模格的后量子密码算法)                          ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* 密钥生成 */
    KyberPublicKey pk;
    KyberPrivateKey sk;
    kyber_keygen(&pk, &sk);

    /* 封装 */
    uint8_t message[32] = {0};
    KyberCiphertext ct;
    uint8_t shared_secret_encap[KYBER_SHARED_SECRET_BYTES];
    kyber_encaps(&pk, message, &ct, shared_secret_encap);

    /* 解封装 */
    uint8_t shared_secret_decap[KYBER_SHARED_SECRET_BYTES];
    kyber_decaps(&sk, &ct, shared_secret_decap);

    printf("封装得到的共享密钥: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X", shared_secret_encap[i]);
    }
    printf("...\n");

    printf("解封装得到的共享密钥: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X", shared_secret_decap[i]);
    }
    printf("...\n");

    printf("\n注：在实际实现中，两个密钥应当相同\n");
    printf("    这是基于MLWE问题的密码学安全性保证\n");

    return 0;
}
