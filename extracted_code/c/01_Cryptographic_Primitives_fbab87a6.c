/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 301
 * Language: c
 * Block ID: fbab87a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用libsodium实现BLAKE2b
#include <sodium.h>

// 初始化
void init_crypto(void) {
    if (sodium_init() < 0) {
        fprintf(stderr, "libsodium初始化失败\n");
        exit(1);
    }
}

// BLAKE2b哈希
int blake2b_hash(const unsigned char* data, size_t data_len,
                 unsigned char hash[crypto_generichash_BYTES]) {
    return crypto_generichash(hash, crypto_generichash_BYTES,
                              data, data_len,
                              NULL, 0);  // 无密钥模式
}

// BLAKE2b带密钥（MAC）
int blake2b_mac(const unsigned char* data, size_t data_len,
                const unsigned char key[crypto_generichash_KEYBYTES],
                unsigned char hash[crypto_generichash_BYTES]) {
    return crypto_generichash(hash, crypto_generichash_BYTES,
                              data, data_len,
                              key, crypto_generichash_KEYBYTES);
}

// 密码哈希（Argon2）
int password_hash(const char* password,
                  unsigned char hash[crypto_pwhash_STRBYTES]) {
    return crypto_pwhash_str(hash, password, strlen(password),
                             crypto_pwhash_OPSLIMIT_INTERACTIVE,
                             crypto_pwhash_MEMLIMIT_INTERACTIVE);
}

int password_verify(const char* password,
                    const unsigned char hash[crypto_pwhash_STRBYTES]) {
    return crypto_pwhash_str_verify(hash, password, strlen(password));
}
