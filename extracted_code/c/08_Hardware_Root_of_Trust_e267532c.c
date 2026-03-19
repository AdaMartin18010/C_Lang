/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 1118
 * Language: c
 * Block ID: e267532c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * PUF密钥派生和密钥层次结构
 */

/* 密钥类型 */
typedef enum {
    PUF_KEY_DEVICE_UNIQUE = 0,      /* 设备唯一密钥 */
    PUF_KEY_ENCRYPTION,             /* 加密密钥 */
    PUF_KEY_AUTHENTICATION,         /* 认证密钥 */
    PUF_KEY_ATTESTATION,            /* 证明密钥 */
    PUF_KEY_WRAPPING,               /* 密钥包装密钥 */
    PUF_KEY_DERIVATION,             /* 密钥派生密钥 */
} puf_key_type_t;

/* 密钥派生上下文 */
typedef struct {
    puf_key_type_t type;
    uint8_t context[32];            /* 派生上下文 */
    uint32_t key_usage;             /* 密钥用途限制 */
    uint32_t valid_from;            /* 有效起始时间 */
    uint32_t valid_until;           /* 有效截止时间 */
} key_derivation_context_t;

/* 派生设备唯一密钥 */
int puf_derive_device_unique_key(uint8_t *key, size_t key_len) {
    /* 1. 从PUF获取根密钥 */
    uint8_t puf_root[64];
    if (sram_puf_reconstruct_key(puf_root, sizeof(puf_root)) != 0) {
        return -EPUF_FAIL;
    }

    /* 2. 派生设备唯一密钥 */
    hkdf_sha256(puf_root, sizeof(puf_root),
                (const uint8_t *)"device_unique", 13,
                NULL, 0,
                key, key_len);

    /* 3. 清除PUF根密钥 */
    secure_memzero(puf_root, sizeof(puf_root));

    return 0;
}

/* 派生应用特定密钥 */
int puf_derive_key(puf_key_type_t type,
                   const uint8_t *context, size_t context_len,
                   uint32_t key_usage,
                   uint8_t *key, size_t key_len) {
    /* 1. 获取设备唯一密钥 */
    uint8_t device_key[32];
    puf_derive_device_unique_key(device_key, sizeof(device_key));

    /* 2. 构建派生上下文 */
    const char *type_str;
    switch (type) {
        case PUF_KEY_ENCRYPTION:        type_str = "enc"; break;
        case PUF_KEY_AUTHENTICATION:    type_str = "auth"; break;
        case PUF_KEY_ATTESTATION:       type_str = "attest"; break;
        case PUF_KEY_WRAPPING:          type_str = "wrap"; break;
        case PUF_KEY_DERIVATION:        type_str = "kdf"; break;
        default:                        type_str = "key"; break;
    }

    /* 3. 派生密钥 */
    hkdf_sha256_expand(device_key, sizeof(device_key),
                       context, context_len,
                       (const uint8_t *)type_str, strlen(type_str),
                       key, key_len);

    /* 4. 清除设备密钥 */
    secure_memzero(device_key, sizeof(device_key));

    return 0;
}

/* PUF密钥层次结构 */
void puf_key_hierarchy_diagram(void) {
    printf("PUF Key Hierarchy:\n");
    printf("\n");
    printf("  PUF Root (物理不可克隆)\n");
    printf("       |\n");
    printf("       v\n");
    printf("  Device Unique Key (DUK)\n");
    printf("       |\n");
    printf("       +--- Application Keys ---+\n");
    printf("       |                        |\n");
    printf("       v                        v\n");
    printf("  Encryption Key          Authentication Key\n");
    printf("       |                        |\n");
    printf("       v                        v\n");
    printf("  Data Encryption         Identity Verification\n");
    printf("  Secure Storage          Secure Communication\n");
}

/* PUF密钥质量测试 */
int puf_key_quality_test(void) {
    /* 1. 重复派生测试 */
    uint8_t key1[32], key2[32];

    puf_derive_key(PUF_KEY_DEVICE_UNIQUE, NULL, 0, 0, key1, 32);
    puf_derive_key(PUF_KEY_DEVICE_UNIQUE, NULL, 0, 0, key2, 32);

    if (memcmp(key1, key2, 32) != 0) {
        ERROR("PUF key derivation not deterministic!\n");
        return -1;
    }

    /* 2. 唯一性测试（需要多个设备）*/
    /* 比较不同设备的密钥 */

    /* 3. 熵测试 */
    double entropy = calculate_entropy(key1, 32);
    if (entropy < 7.9) {  /* 期望接近8 */
        WARN("Low entropy in derived key: %f\n", entropy);
    }

    /* 4. 统计随机性测试 */
    if (run_nist_sp800_22_tests(key1, 32) != 0) {
        WARN("Derived key failed NIST randomness tests\n");
    }

    return 0;
}
