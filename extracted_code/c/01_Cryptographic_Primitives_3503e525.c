/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 461
 * Language: c
 * Block ID: 3503e525
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// libsodium实现
#include <sodium.h>

#define CHACHA20_KEY_SIZE crypto_aead_chacha20poly1305_IETF_KEYBYTES
#define CHACHA20_NONCE_SIZE crypto_aead_chacha20poly1305_IETF_NPUBBYTES
#define CHACHA20_TAG_SIZE crypto_aead_chacha20poly1305_IETF_ABYTES

int chacha20_encrypt(const unsigned char key[CHACHA20_KEY_SIZE],
                     const unsigned char nonce[CHACHA20_NONCE_SIZE],
                     const unsigned char* plaintext, size_t pt_len,
                     const unsigned char* aad, size_t aad_len,
                     unsigned char* ciphertext,
                     unsigned char tag[CHACHA20_TAG_SIZE]) {
    unsigned long long ciphertext_len;
    int ret = crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext, &ciphertext_len,
        plaintext, pt_len,
        aad, aad_len,
        NULL,  // nsec（未使用）
        nonce, key);

    if (ret == 0) {
        // 分离标签（最后16字节）
        memcpy(tag, ciphertext + pt_len, CHACHA20_TAG_SIZE);
    }

    return ret;
}

// ChaCha20优于AES的场景：
// 1. 无AES-NI硬件的旧CPU
// 2. 移动设备（ARM）
// 3. 需要恒定时间实现
