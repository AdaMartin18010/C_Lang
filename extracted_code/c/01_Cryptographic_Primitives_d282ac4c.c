/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 569
 * Language: c
 * Block ID: d282ac4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ECDSA (secp256k1 - 比特币使用)
#include <openssl/ec.h>

int ecdsa_sign(const unsigned char* hash, size_t hash_len,
               unsigned char* sig, size_t* sig_len,
               EC_KEY* ec_key) {
    ECDSA_SIG* signature = ECDSA_do_sign(hash, hash_len, ec_key);
    if (!signature) return -1;

    // 序列化为DER
    unsigned char* sig_ptr = sig;
    *sig_len = i2d_ECDSA_SIG(signature, &sig_ptr);

    ECDSA_SIG_free(signature);
    return 0;
}

int ecdsa_verify(const unsigned char* hash, size_t hash_len,
                 const unsigned char* sig, size_t sig_len,
                 EC_KEY* ec_key) {
    const unsigned char* p = sig;
    ECDSA_SIG* signature = d2i_ECDSA_SIG(NULL, &p, sig_len);
    if (!signature) return -1;

    int ret = ECDSA_do_verify(hash, hash_len, signature, ec_key);
    ECDSA_SIG_free(signature);
    return ret;  // 1=valid, 0=invalid, -1=error
}

// Ed25519（libsodium - 推荐用于新设计）
void ed25519_example(void) {
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    // 生成密钥对
    crypto_sign_keypair(pk, sk);

    // 签名
    const char* message = "Hello, World!";
    unsigned char signed_msg[crypto_sign_BYTES + 13];
    unsigned long long signed_len;
    crypto_sign(signed_msg, &signed_len,
                (const unsigned char*)message, strlen(message),
                sk);

    // 验证
    unsigned char verified_msg[13];
    unsigned long long verified_len;
    int ret = crypto_sign_open(verified_msg, &verified_len,
                               signed_msg, signed_len,
                               pk);
    if (ret == 0) {
        printf("签名验证成功\n");
    }
}
