/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 1630
 * Language: c
 * Block ID: d40cd3e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全密钥注入流程
 */

/* 注入阶段 */
typedef enum {
    INJECT_PHASE_PREPARATION = 0,
    INJECT_PHASE_AUTHENTICATION,
    INJECT_PHASE_KEY_TRANSFER,
    INJECT_PHASE_VERIFICATION,
    INJECT_PHASE_FINALIZATION,
} key_inject_phase_t;

/* 密钥包 */
typedef struct __attribute__((packed)) {
    uint32_t magic;                 /* "KEYP" */
    uint32_t version;
    uint32_t key_type;
    uint32_t key_algorithm;
    uint32_t key_size;
    uint32_t encrypted_key_offset;
    uint32_t encrypted_key_size;
    uint32_t wrapping_pubkey_offset;
    uint32_t wrapping_pubkey_size;
    uint32_t signature_offset;
    uint32_t signature_size;
    uint8_t  metadata[64];
    /* 后跟：加密密钥、包装公钥、签名 */
} key_package_header_t;

#define KEY_PACKAGE_MAGIC   0x5059454B  /* "KEYP" */

/* HSM上下文 */
typedef struct {
    void *session;
    uint8_t hsm_pubkey[64];         /* HSM公钥（用于验证）*/
    uint32_t key_slot;
} hsm_context_t;

/* 密钥注入流程 */
int inject_device_key(const uint8_t *key_package, size_t package_size) {
    key_inject_phase_t phase = INJECT_PHASE_PREPARATION;

    /* 1. 准备阶段 */
    phase = INJECT_PHASE_PREPARATION;

    /* 验证设备处于工厂模式 */
    if (!is_factory_mode()) {
        return -ENOTFACTORY;
    }

    /* 验证密钥包格式 */
    if (package_size < sizeof(key_package_header_t)) {
        return -EINVALID_PACKAGE;
    }

    const key_package_header_t *header = (const key_package_header_t *)key_package;
    if (header->magic != KEY_PACKAGE_MAGIC) {
        return -EINVALID_MAGIC;
    }

    /* 2. 认证阶段 */
    phase = INJECT_PHASE_AUTHENTICATION;

    /* 验证HSM签名 */
    const uint8_t *signature = key_package + header->signature_offset;
    const uint8_t *hsm_pubkey = key_package + header->wrapping_pubkey_offset;

    if (ed25519_verify(signature,
                       key_package,
                       header->signature_offset,
                       hsm_pubkey) != 0) {
        return -EAUTH_FAILED;
    }

    /* 3. 密钥传输阶段 */
    phase = INJECT_PHASE_KEY_TRANSFER;

    /* 加载设备私钥（用于ECDH） */
    uint8_t device_privkey[32];
    load_device_keypair_priv(device_privkey);

    /* 执行ECDH密钥协商 */
    uint8_t shared_secret[32];
    x25519(shared_secret, device_privkey, hsm_pubkey);

    /* 派生解包密钥 */
    uint8_t unwrapping_key[32];
    hkdf_sha256(shared_secret, sizeof(shared_secret),
                NULL, 0,
                (const uint8_t *)"key_unwrap", 10,
                unwrapping_key, sizeof(unwrapping_key));

    /* 解密密钥 */
    const uint8_t *encrypted_key = key_package + header->encrypted_key_offset;
    uint8_t decrypted_key[64];

    aes_gcm_decrypt(unwrapping_key,
                    encrypted_key + header->encrypted_key_size - 16,  /* IV在最后 */
                    encrypted_key, header->encrypted_key_size - 16 - 16,  /* 密文 */
                    NULL, 0,
                    decrypted_key,
                    encrypted_key + header->encrypted_key_size - 16);  /* TAG */

    /* 4. 验证阶段 */
    phase = INJECT_PHASE_VERIFICATION;

    /* 验证解密密钥的格式 */
    if (!validate_key_format(decrypted_key, header->key_type, header->key_size)) {
        secure_memzero(decrypted_key, sizeof(decrypted_key));
        return -EINVALID_KEY;
    }

    /* 5. 最终阶段 */
    phase = INJECT_PHASE_FINALIZATION;

    /* 将密钥写入安全存储 */
    if (store_key_securely(header->key_type, decrypted_key, header->key_size) != 0) {
        secure_memzero(decrypted_key, sizeof(decrypted_key));
        return -ESTORE_FAILED;
    }

    /* 记录密钥注入事件 */
    log_key_injection(header->key_type, hash_key(decrypted_key, header->key_size));

    /* 清理 */
    secure_memzero(shared_secret, sizeof(shared_secret));
    secure_memzero(unwrapping_key, sizeof(unwrapping_key));
    secure_memzero(decrypted_key, sizeof(decrypted_key));

    return 0;
}
