/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 1550
 * Language: c
 * Block ID: f7563856
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef USE_OPENSSL
#include <openssl/evp.h>
#include <openssl/sha.h>

// 安全哈希计算
typedef enum {
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512
} hash_type_t;

bool compute_hash(const void *data, size_t len, 
                  hash_type_t type, 
                  unsigned char *out, size_t *out_len) {
    const EVP_MD *md;
    switch (type) {
        case HASH_SHA256: md = EVP_sha256(); *out_len = 32; break;
        case HASH_SHA384: md = EVP_sha384(); *out_len = 48; break;
        case HASH_SHA512: md = EVP_sha512(); *out_len = 64; break;
        default: return false;
    }
    
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return false;
    
    bool success = EVP_DigestInit_ex(ctx, md, NULL) &&
                   EVP_DigestUpdate(ctx, data, len) &&
                   EVP_DigestFinal_ex(ctx, out, NULL);
    
    EVP_MD_CTX_free(ctx);
    return success;
}

// HMAC计算
bool compute_hmac(const void *key, size_t key_len,
                  const void *data, size_t data_len,
                  hash_type_t type,
                  unsigned char *out, size_t *out_len) {
    const EVP_MD *md;
    switch (type) {
        case HASH_SHA256: md = EVP_sha256(); *out_len = 32; break;
        case HASH_SHA384: md = EVP_sha384(); *out_len = 48; break;
        case HASH_SHA512: md = EVP_sha512(); *out_len = 64; break;
        default: return false;
    }
    
    unsigned int len;
    HMAC(md, key, (int)key_len, data, data_len, out, &len);
    *out_len = len;
    return true;
}

// PBKDF2密钥派生
bool derive_key(const char *password, const void *salt, size_t salt_len,
                int iterations, hash_type_t type,
                unsigned char *out, size_t out_len) {
    const EVP_MD *md;
    switch (type) {
        case HASH_SHA256: md = EVP_sha256(); break;
        case HASH_SHA384: md = EVP_sha384(); break;
        case HASH_SHA512: md = EVP_sha512(); break;
        default: return false;
    }
    
    return PKCS5_PBKDF2_HMAC(password, -1, salt, (int)salt_len, 
                             iterations, md, (int)out_len, out) == 1;
}
#endif

// 简单哈希（非密码学安全，仅用于数据结构）
#include <stdint.h>

// FNV-1a哈希（快速，非加密）
uint32_t fnv1a_hash(const void *data, size_t len) {
    const uint32_t FNV_PRIME = 0x01000193;
    const uint32_t FNV_OFFSET = 0x811c9dc5;
    
    const unsigned char *p = data;
    uint32_t hash = FNV_OFFSET;
    
    for (size_t i = 0; i < len; i++) {
        hash ^= p[i];
        hash *= FNV_PRIME;
    }
    
    return hash;
}
