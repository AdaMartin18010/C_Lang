/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 444
 * Language: c
 * Block ID: e5d1701c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * PQC算法选择决策框架
 */

typedef struct {
    uint32_t min_security_bits;     /* 最小安全强度 */
    uint32_t max_latency_us;        /* 最大延迟（微秒） */
    uint32_t max_ram_bytes;         /* 最大RAM限制 */
    uint32_t max_code_bytes;        /* 最大代码大小 */
    bool need_small_signatures;     /* 需要小签名 */
    bool need_small_keys;           /* 需要小密钥 */
    bool need_fast_signing;         /* 需要快速签名 */
    bool need_fast_verification;    /* 需要快速验签 */
} pqc_requirements_t;

/* 算法评分 */
typedef struct {
    const char *alg_name;
    uint32_t security_score;        /* 安全性评分 */
    uint32_t performance_score;     /* 性能评分 */
    uint32_t size_score;            /* 大小评分 */
    uint32_t overall_score;         /* 综合评分 */
} alg_score_t;

/* 推荐算法选择 */
const char* recommend_algorithm(const pqc_requirements_t *reqs) {
    if (reqs->need_small_signatures && reqs->need_fast_verification) {
        /* 验证频繁、签名大小敏感场景 */
        return "ML-DSA-65";
    }

    if (reqs->max_ram_bytes < 10000) {
        /* 极度内存受限 */
        return "SLH-DSA-SHA2-128s";
    }

    if (reqs->need_small_keys) {
        /* 密钥传输受限 */
        return "SLH-DSA-SHA2-128s";
    }

    if (reqs->min_security_bits >= 192) {
        /* 高安全需求 */
        return "ML-DSA-87";
    }

    /* 默认推荐 */
    return "ML-DSA-65";
}
