/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 1767
 * Language: c
 * Block ID: b937a910
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全密钥存储实现
 */

/* 存储区域类型 */
typedef enum {
    STORAGE_EFUSE = 0,          /* eFuse（一次性）*/
    STORAGE_OTP,                /* OTP存储 */
    STORAGE_SECURE_FLASH,       /* 加密Flash */
    STORAGE_TPM_NV,             /* TPM非易失存储 */
    STORAGE_SECURE_ELEMENT,     /* 外部安全元件 */
} storage_type_t;

/* 密钥存储元数据 */
typedef struct {
    uint32_t key_id;
    storage_type_t storage;
    uint32_t key_size;
    uint32_t key_usage;
    uint32_t creation_time;
    uint32_t expiration_time;
    uint8_t access_policy;
    bool is_exportable;
} key_metadata_t;

/* 安全存储上下文 */
typedef struct {
    uint8_t encryption_key[32];
    uint8_t authentication_key[32];
    uint32_t monotonic_counter;
} secure_storage_ctx_t;

static secure_storage_ctx_t g_storage_ctx;

/* 初始化安全存储 */
int secure_storage_init(void) {
    /* 从PUF派生存储密钥 */
    puf_derive_key(PUF_KEY_WRAPPING, NULL, 0, 0,
                   g_storage_ctx.encryption_key, 32);

    puf_derive_key(PUF_KEY_AUTHENTICATION, NULL, 0, 0,
                   g_storage_ctx.authentication_key, 32);

    /* 读取单调计数器 */
    g_storage_ctx.monotonic_counter = read_monotonic_counter();

    return 0;
}

/* 安全存储密钥 */
int store_key_securely(uint32_t key_id,
                       const uint8_t *key_data,
                       size_t key_len,
                       const key_metadata_t *metadata) {
    /* 1. 准备存储结构 */
    typedef struct __attribute__((packed)) {
        key_metadata_t metadata;
        uint8_t encrypted_key[256];
        uint8_t iv[16];
        uint8_t tag[16];
        uint32_t counter;
        uint8_t hmac[32];
    } stored_key_t;

    stored_key_t stored = {0};

    /* 2. 复制元数据 */
    stored.metadata = *metadata;

    /* 3. 生成IV */
    secure_random(stored.iv, sizeof(stored.iv));

    /* 4. 增加单调计数器 */
    stored.counter = ++g_storage_ctx.monotonic_counter;
    update_monotonic_counter(stored.counter);

    /* 5. 加密密钥 */
    aes_gcm_encrypt(g_storage_ctx.encryption_key,
                    stored.iv,
                    key_data, key_len,
                    (const uint8_t *)&stored.metadata, sizeof(stored.metadata),
                    stored.encrypted_key,
                    stored.tag);

    /* 6. 计算HMAC */
    hmac_sha256(g_storage_ctx.authentication_key,
                (const uint8_t *)&stored,
                offsetof(stored_key_t, hmac),
                stored.hmac);

    /* 7. 写入存储 */
    if (metadata->storage == STORAGE_EFUSE) {
        return write_to_efuse(key_id, (const uint8_t *)&stored, sizeof(stored));
    } else if (metadata->storage == STORAGE_SECURE_FLASH) {
        return write_to_secure_flash(key_id, (const uint8_t *)&stored, sizeof(stored));
    }

    return -EUNSUPPORTED_STORAGE;
}

/* 安全读取密钥 */
int load_key_securely(uint32_t key_id,
                      uint8_t *key_data,
                      size_t *key_len,
                      key_metadata_t *metadata) {
    /* 1. 从存储读取 */
    uint8_t stored_data[512];
    size_t stored_len;

    if (read_from_storage(key_id, stored_data, &stored_len) != 0) {
        return -ENOTFOUND;
    }

    typedef struct __attribute__((packed)) {
        key_metadata_t metadata;
        uint8_t encrypted_key[256];
        uint8_t iv[16];
        uint8_t tag[16];
        uint32_t counter;
        uint8_t hmac[32];
    } stored_key_t;

    stored_key_t *stored = (stored_key_t *)stored_data;

    /* 2. 验证HMAC */
    uint8_t computed_hmac[32];
    hmac_sha256(g_storage_ctx.authentication_key,
                stored_data,
                offsetof(stored_key_t, hmac),
                computed_hmac);

    if (memcmp(computed_hmac, stored->hmac, 32) != 0) {
        return -ETAMPERED;
    }

    /* 3. 检查单调计数器（防回滚）*/
    if (stored->counter > g_storage_ctx.monotonic_counter) {
        /* 可能发生了回滚攻击 */
        return -EROLLBACK;
    }

    /* 4. 解密密钥 */
    *key_len = stored->metadata.key_size;

    if (!aes_gcm_decrypt(g_storage_ctx.encryption_key,
                         stored->iv,
                         stored->encrypted_key, stored->metadata.key_size,
                         (const uint8_t *)&stored->metadata, sizeof(stored->metadata),
                         key_data,
                         stored->tag)) {
        return -EDECRYPT_FAIL;
    }

    /* 5. 返回元数据 */
    if (metadata) {
        *metadata = stored->metadata;
    }

    return 0;
}
