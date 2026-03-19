/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1938
 * Language: c
 * Block ID: 5cf426ec
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全密钥注入流程
 * 工厂阶段的密钥配置
 */

/* 密钥注入状态 */
typedef enum {
    KEY_INJECT_IDLE = 0,
    KEY_INJECT_PREPARED,
    KEY_INJECT_IN_PROGRESS,
    KEY_INJECT_COMPLETED,
    KEY_INJECT_FAILED,
} key_inject_state_t;

/* 密钥包结构（加密） */
typedef struct __attribute__((packed)) {
    uint32_t magic;                 /* "KEYP" */
    uint32_t version;
    uint8_t  encrypted_key[256];    /* 加密密钥 */
    uint8_t  iv[16];                /* 初始化向量 */
    uint8_t  auth_tag[16];          /* 认证标签 */
    uint8_t  wrapping_pubkey[256];  /* 包装公钥 */
    uint32_t key_type;
    uint32_t key_attributes;
} key_package_t;

#define KEY_PACKAGE_MAGIC   0x5059454B  /* "KEYP" */

/* HSM连接（工厂环境） */
typedef struct {
    void *hsm_session;
    uint8_t hsm_pubkey[256];
    uint32_t key_slot;
} hsm_context_t;

/* 生成密钥包 */
int generate_key_package(hsm_context_t *hsm,
                         key_type_t key_type,
                         const uint8_t *device_pubkey,
                         uint32_t device_pubkey_len,
                         key_package_t *package) {

    /* 1. 在HSM中生成临时密钥 */
    uint8_t ephemeral_key[32];
    hsm_generate_ephemeral_key(hsm, ephemeral_key, sizeof(ephemeral_key));

    /* 2. 执行ECDH密钥协商 */
    uint8_t shared_secret[32];
    hsm_ecdh(hsm, ephemeral_key, device_pubkey, shared_secret);

    /* 3. 派生包装密钥 */
    uint8_t wrapping_key[32];
    HKDF_SHA256(shared_secret, sizeof(shared_secret),
                NULL, 0,
                (const uint8_t *)"key_wrapping", 11,
                wrapping_key, sizeof(wrapping_key));

    /* 4. 在HSM中生成目标密钥 */
    uint8_t target_key[32];
    hsm_generate_key(hsm, key_type, target_key, sizeof(target_key));

    /* 5. 加密目标密钥 */
    package->magic = KEY_PACKAGE_MAGIC;
    package->version = 1;
    package->key_type = key_type;

    /* 生成IV */
    hsm_get_random(hsm, package->iv, sizeof(package->iv));

    /* AES-256-GCM加密 */
    aes_gcm_encrypt(wrapping_key, package->iv,
                    target_key, sizeof(target_key),
                    (uint8_t *)&package->key_type, sizeof(package->key_type),
                    package->encrypted_key,
                    package->auth_tag);

    /* 6. 复制包装公钥 */
    hsm_get_pubkey(hsm, ephemeral_key, package->wrapping_pubkey);

    /* 7. 清理敏感数据 */
    secure_memzero(ephemeral_key, sizeof(ephemeral_key));
    secure_memzero(shared_secret, sizeof(shared_secret));
    secure_memzero(wrapping_key, sizeof(wrapping_key));
    secure_memzero(target_key, sizeof(target_key));

    return 0;
}

/* 设备端密钥解包和安装 */
int install_key_package(const key_package_t *package,
                        const uint8_t *device_privkey) {
    /* 1. 验证包格式 */
    if (package->magic != KEY_PACKAGE_MAGIC) {
        return -EINVAL;
    }

    /* 2. 执行ECDH（使用设备私钥） */
    uint8_t shared_secret[32];
    ecdh_shared_secret(device_privkey, package->wrapping_pubkey, shared_secret);

    /* 3. 派生包装密钥 */
    uint8_t wrapping_key[32];
    HKDF_SHA256(shared_secret, sizeof(shared_secret),
                NULL, 0,
                (const uint8_t *)"key_wrapping", 11,
                wrapping_key, sizeof(wrapping_key));

    /* 4. 解密并验证密钥 */
    uint8_t decrypted_key[32];
    uint8_t computed_tag[16];

    aes_gcm_decrypt(wrapping_key, package->iv,
                    package->encrypted_key, sizeof(decrypted_key),
                    (uint8_t *)&package->key_type, sizeof(package->key_type),
                    decrypted_key,
                    computed_tag);

    /* 5. 验证认证标签 */
    if (memcmp(computed_tag, package->auth_tag, 16) != 0) {
        secure_memzero(shared_secret, sizeof(shared_secret));
        secure_memzero(wrapping_key, sizeof(wrapping_key));
        return -EINTEGRITY;
    }

    /* 6. 安全存储密钥（eFuse/TPM/安全存储） */
    switch (package->key_type) {
        case KEY_TYPE_DEVICE:
            /* 写入eFuse */
            efuse_program(EFUSE_DUK_BLOCK, decrypted_key, sizeof(decrypted_key));
            break;

        case KEY_TYPE_STORAGE:
            /* 密封到TPM */
            seal_key_to_tpm(decrypted_key, sizeof(decrypted_key),
                           PCR_BOOT_LOADER, package->key_attributes);
            break;

        default:
            break;
    }

    /* 7. 清理 */
    secure_memzero(shared_secret, sizeof(shared_secret));
    secure_memzero(wrapping_key, sizeof(wrapping_key));
    secure_memzero(decrypted_key, sizeof(decrypted_key));

    return 0;
}
