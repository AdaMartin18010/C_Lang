/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\01_ARM_Trusted_Firmware.md
 * Line: 304
 * Language: c
 * Block ID: bb300807
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RSA-2048签名验证

#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/pem.h>

#define RSA_KEY_SIZE    2048
#define RSA_SIG_SIZE    (RSA_KEY_SIZE / 8)  // 256 bytes

// 公钥结构
typedef struct {
    uint8_t n[RSA_SIG_SIZE];  // 模数
    uint8_t e[4];             // 指数 (通常65537 = 0x10001)
} RsaPublicKey;

// 验证镜像签名
bool verify_image_rsa(const uint8_t *image, size_t image_size,
                      const uint8_t *signature,
                      const RsaPublicKey *pub_key) {
    // 1. 计算镜像哈希 (SHA-256)
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(image, image_size, hash);

    // 2. RSA验证签名
    RSA *rsa = RSA_new();
    BIGNUM *n = BN_bin2bn(pub_key->n, RSA_SIG_SIZE, NULL);
    BIGNUM *e = BN_bin2bn(pub_key->e, sizeof(pub_key->e), NULL);
    RSA_set0_key(rsa, n, e, NULL);

    // 3. 验证
    int ret = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH,
                         signature, RSA_SIG_SIZE, rsa);

    RSA_free(rsa);
    return ret == 1;
}

// 简化的哈希链验证（用于资源受限环境）
#define HASH_SIZE 32

typedef struct {
    uint8_t hash[HASH_SIZE];
} HashNode;

// 计算组合哈希
void hash_combine(uint8_t *out, const uint8_t *left, const uint8_t *right) {
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, left, HASH_SIZE);
    SHA256_Update(&ctx, right, HASH_SIZE);
    SHA256_Final(out, &ctx);
}

// Merkle树根验证
bool verify_merkle_root(const uint8_t *root,
                        const uint8_t *leaf,
                        size_t leaf_index,
                        const uint8_t **proof,
                        size_t proof_len) {
    uint8_t current[HASH_SIZE];
    memcpy(current, leaf, HASH_SIZE);

    for (size_t i = 0; i < proof_len; i++) {
        uint8_t combined[HASH_SIZE];
        if (leaf_index & (1 << i)) {
            // 当前节点在右侧
            hash_combine(combined, proof[i], current);
        } else {
            // 当前节点在左侧
            hash_combine(combined, current, proof[i]);
        }
        memcpy(current, combined, HASH_SIZE);
    }

    return memcmp(current, root, HASH_SIZE) == 0;
}
