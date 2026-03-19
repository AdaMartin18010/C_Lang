/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\README.md
 * Line: 520
 * Language: c
 * Block ID: c16cda39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 安全启动密钥层次结构 */
#define KEY_HIERARCHY_LAYERS 4

typedef enum {
    KEY_LAYER_ROOT = 0,        /* 根密钥 - 离线存储 */
    KEY_LAYER_INTERMEDIATE,    /* 中间CA密钥 */
    KEY_LAYER_SIGNING,         /* 签名密钥 */
    KEY_LAYER_DEVICE           /* 设备特定密钥 */
} key_layer_t;

/* 密钥元数据 */
typedef struct {
    key_layer_t layer;
    uint32_t key_id;
    uint32_t key_version;
    uint64_t creation_time;
    uint64_t expiration_time;
    uint32_t usage_flags;      /* 签名/加密/认证 */
    uint8_t key_hash[32];      /* 公钥哈希 */
} key_metadata_t;

/* 安全密钥存储 */
typedef struct {
    key_metadata_t meta;
    uint8_t encrypted_key[4096];   /* AES-256-GCM加密 */
    uint8_t key_iv[12];
    uint8_t key_tag[16];
    uint8_t wrapping_key_hash[32]; /* 用于解封的父密钥哈希 */
} secure_key_blob_t;

/* 密钥生成与存储 */
int generate_secure_key(key_layer_t layer, secure_key_blob_t *out_key) {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    /* 在HSM/TPM中生成密钥对 */
    if (layer == KEY_LAYER_ROOT || layer == KEY_LAYER_INTERMEDIATE) {
        /* 使用HSM生成4096位RSA密钥 */
        EVP_PKEY_keygen_init(ctx);
        EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 4096);
        EVP_PKEY_keygen(ctx, &pkey);
    } else {
        /* 使用TPM生成2048位RSA密钥 */
        pkey = tpm2_generate_key(2048);
    }

    /* 使用AES-256-GCM加密私钥 */
    uint8_t aes_key[32], iv[12], tag[16];
    get_random_bytes(aes_key, 32);
    get_random_bytes(iv, 12);

    encrypt_aes_gcm(private_key_der, private_key_len,
                    aes_key, iv, out_key->encrypted_key, tag);

    /* 使用上层密钥加密AES密钥 */
    encrypt_with_parent_key(aes_key, 32, layer - 1,
                            out_key->wrapping_key_hash);

    memcpy(out_key->key_iv, iv, 12);
    memcpy(out_key->key_tag, tag, 16);

    /* 填充元数据 */
    out_key->meta.layer = layer;
    out_key->meta.key_id = get_next_key_id();
    out_key->meta.creation_time = get_timestamp();
    out_key->meta.expiration_time = out_key->meta.creation_time +
                                    (365 * 24 * 3600);  /* 1年有效期 */

    /* 计算公钥哈希 */
    SHA256(public_key_der, public_key_len, out_key->meta.key_hash);

    EVP_PKEY_CTX_free(ctx);
    return 0;
}
