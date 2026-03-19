/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 137
 * Language: c
 * Block ID: cb6f7b0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 量子计算对现有密码学的影响分析
 */

#include <stdint.h>
#include <stdbool.h>

/* 密码算法类型 */
typedef enum {
    ALG_RSA,
    ALG_ECDSA,
    ALG_ECDH,
    ALG_DSA,
    ALG_AES_128,
    ALG_AES_256,
    ALG_SHA_256,
    ALG_SHA_3,
} crypto_alg_t;

/* 量子威胁级别 */
typedef enum {
    THREAT_NONE = 0,        /* 不受影响 */
    THREAT_REDUCED,         /* 安全性降低 */
    THREAT_BROKEN,          /* 完全破解 */
} quantum_threat_t;

/* 算法影响评估 */
typedef struct {
    crypto_alg_t algorithm;
    const char *name;
    quantum_threat_t threat_level;
    const char *reason;
    uint32_t classical_security_bits;
    uint32_t quantum_security_bits;
} alg_impact_t;

/* 影响评估表 */
static const alg_impact_t quantum_impacts[] = {
    {
        .algorithm = ALG_RSA,
        .name = "RSA (1024-4096 bit)",
        .threat_level = THREAT_BROKEN,
        .reason = "Shor算法可因数分解",
        .classical_security_bits = 80,
        .quantum_security_bits = 0,
    },
    {
        .algorithm = ALG_ECDSA,
        .name = "ECDSA/ECDH (P-256, P-384)",
        .threat_level = THREAT_BROKEN,
        .reason = "Shor算法可解离散对数",
        .classical_security_bits = 128,
        .quantum_security_bits = 0,
    },
    {
        .algorithm = ALG_AES_128,
        .name = "AES-128",
        .threat_level = THREAT_REDUCED,
        .reason = "Grover算法将安全性降为64位",
        .classical_security_bits = 128,
        .quantum_security_bits = 64,
    },
    {
        .algorithm = ALG_AES_256,
        .name = "AES-256",
        .threat_level = THREAT_REDUCED,
        .reason = "Grover算法将安全性降为128位",
        .classical_security_bits = 256,
        .quantum_security_bits = 128,
    },
    {
        .algorithm = ALG_SHA_256,
        .name = "SHA-256/SHA-3",
        .threat_level = THREAT_REDUCED,
        .reason = "碰撞攻击速度平方根提升",
        .classical_security_bits = 256,
        .quantum_security_bits = 128,
    },
};

/* 检查算法是否量子安全 */
bool is_quantum_safe(crypto_alg_t alg) {
    for (size_t i = 0; i < sizeof(quantum_impacts)/sizeof(quantum_impacts[0]); i++) {
        if (quantum_impacts[i].algorithm == alg) {
            return quantum_impacts[i].threat_level == THREAT_NONE;
        }
    }
    /* 未知算法，保守处理 */
    return false;
}

/* 获取安全降级因子 */
double get_security_degradation(crypto_alg_t alg) {
    for (size_t i = 0; i < sizeof(quantum_impacts)/sizeof(quantum_impacts[0]); i++) {
        if (quantum_impacts[i].algorithm == alg) {
            if (quantum_impacts[i].quantum_security_bits == 0) {
                return 0.0;  /* 完全破解 */
            }
            return (double)quantum_impacts[i].quantum_security_bits /
                   quantum_impacts[i].classical_security_bits;
        }
    }
    return 0.5;  /* 默认假设减半 */
}
