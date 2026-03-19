/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\02_Secure_Boot_Chain.md
 * Line: 383
 * Language: c
 * Block ID: 31f81851
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 密钥存储结构
typedef struct {
    uint32_t key_version;     // 密钥版本
    uint32_t valid_from;      // 生效时间
    uint32_t valid_until;     // 过期时间
    uint8_t  revoked;         // 吊销标志
    uint8_t  key_hash[32];    // 公钥哈希
} KeyMetadata;

// 密钥吊销列表 (CRL)
typedef struct {
    uint32_t version;
    uint32_t entry_count;
    uint32_t revoked_keys[32];  // 吊销的密钥版本
} RevocationList;

// 验证密钥有效性
int verify_key_valid(const KeyMetadata *key, const RevocationList *crl) {
    // 检查吊销
    for (int i = 0; i < crl->entry_count; i++) {
        if (crl->revoked_keys[i] == key->key_version) {
            printf("Key version %u revoked!\n", key->key_version);
            return -1;
        }
    }

    // 检查有效期
    uint32_t current_time = get_current_timestamp();
    if (current_time < key->valid_from || current_time > key->valid_until) {
        printf("Key outside validity period\n");
        return -1;
    }

    return 0;
}

// 安全密钥更新
int update_public_key(uint32_t key_version, const uint8_t *new_key_hash,
                      const uint8_t *signature) {
    // 验证更新签名（使用当前密钥）
    if (!verify_update_signature(key_version, new_key_hash, signature)) {
        return -1;
    }

    // 写入OTP（一次性）或安全存储
    write_otp_key_hash(key_version, new_key_hash);

    return 0;
}
