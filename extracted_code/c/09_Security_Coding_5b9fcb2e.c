/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 177
 * Language: c
 * Block ID: 5b9fcb2e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    uint8_t key[32];
    uint8_t nonce[12];
} SymmetricKey;

// 加密
bool encrypt_aes_gcm(const uint8_t *plaintext, size_t pt_len,
                     const SymmetricKey *key,
                     const uint8_t *aad, size_t aad_len,
                     uint8_t *ciphertext,
                     uint8_t tag[16]);

// 解密
bool decrypt_aes_gcm(const uint8_t *ciphertext, size_t ct_len,
                     const SymmetricKey *key,
                     const uint8_t *aad, size_t aad_len,
                     const uint8_t tag[16],
                     uint8_t *plaintext);
