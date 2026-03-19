/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 282
 * Language: c
 * Block ID: f2f15293
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 后量子密码算法分类和特性
 */

/* PQC算法家族 */
typedef enum {
    PQC_LATTICE,        /* 格密码学 */
    PQC_HASH,           /* 哈希密码学 */
    PQC_CODE,           /* 编码密码学 */
    PQC_MULTIVARIATE,   /* 多变元密码学 */
    PQC_ISOGENY,        /* 同源密码学 */
} pqc_family_t;

/* 算法特性结构 */
typedef struct {
    const char *name;
    pqc_family_t family;
    bool is_kem;                    /* 密钥封装 */
    bool is_signature;              /* 数字签名 */
    uint32_t public_key_bytes;
    uint32_t secret_key_bytes;
    uint32_t ciphertext_bytes;      /* KEM密文 */
    uint32_t signature_bytes;
    uint32_t security_level;        /* NIST级别 1/3/5 */
    bool nist_standardized;         /* 是否NIST标准 */
} pqc_algorithm_t;

/* NIST标准化算法（截至2024） */
static const pqc_algorithm_t nist_algorithms[] = {
    /* KEM */
    {
        .name = "ML-KEM-512",
        .family = PQC_LATTICE,
        .is_kem = true,
        .public_key_bytes = 800,
        .secret_key_bytes = 1632,
        .ciphertext_bytes = 768,
        .security_level = 1,
        .nist_standardized = true,
    },
    {
        .name = "ML-KEM-768",
        .family = PQC_LATTICE,
        .is_kem = true,
        .public_key_bytes = 1184,
        .secret_key_bytes = 2400,
        .ciphertext_bytes = 1088,
        .security_level = 3,
        .nist_standardized = true,
    },
    {
        .name = "ML-KEM-1024",
        .family = PQC_LATTICE,
        .is_kem = true,
        .public_key_bytes = 1568,
        .secret_key_bytes = 3168,
        .ciphertext_bytes = 1568,
        .security_level = 5,
        .nist_standardized = true,
    },

    /* 签名 */
    {
        .name = "ML-DSA-44",
        .family = PQC_LATTICE,
        .is_signature = true,
        .public_key_bytes = 1312,
        .secret_key_bytes = 2560,
        .signature_bytes = 2420,
        .security_level = 2,
        .nist_standardized = true,
    },
    {
        .name = "ML-DSA-65",
        .family = PQC_LATTICE,
        .is_signature = true,
        .public_key_bytes = 1952,
        .secret_key_bytes = 4032,
        .signature_bytes = 3293,
        .security_level = 3,
        .nist_standardized = true,
    },
    {
        .name = "ML-DSA-87",
        .family = PQC_LATTICE,
        .is_signature = true,
        .public_key_bytes = 2592,
        .secret_key_bytes = 4896,
        .signature_bytes = 4595,
        .security_level = 5,
        .nist_standardized = true,
    },
    {
        .name = "SLH-DSA-SHA2-128s",
        .family = PQC_HASH,
        .is_signature = true,
        .public_key_bytes = 32,
        .secret_key_bytes = 64,
        .signature_bytes = 7856,
        .security_level = 1,
        .nist_standardized = true,
    },
    {
        .name = "SLH-DSA-SHA2-256f",
        .family = PQC_HASH,
        .is_signature = true,
        .public_key_bytes = 64,
        .secret_key_bytes = 128,
        .signature_bytes = 49856,
        .security_level = 5,
        .nist_standardized = true,
    },
};

/* 算法比较函数 */
const pqc_algorithm_t* find_algorithm(const char *name) {
    for (size_t i = 0; i < sizeof(nist_algorithms)/sizeof(nist_algorithms[0]); i++) {
        if (strcmp(nist_algorithms[i].name, name) == 0) {
            return &nist_algorithms[i];
        }
    }
    return NULL;
}
