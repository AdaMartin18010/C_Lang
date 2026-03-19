/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 1551
 * Language: c
 * Block ID: fe1a4afd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 混合加密方案
 * 结合经典和后量子算法提供过渡安全
 */

#include <openssl/evp.h>

/* 混合KEM */
typedef struct {
    const EVP_PKEY *classic_kem;    /* 经典算法（如X25519） */
    const void *pq_kem;              /* PQC算法（ML-KEM-768） */
} hybrid_kem_t;

/* 混合密钥封装 */
int hybrid_encapsulate(uint8_t *ct, uint8_t *ss,
                       const hybrid_kem_t *hk) {
    uint8_t classic_ct[256];
    uint8_t classic_ss[32];
    uint8_t pq_ct[ML_KEM_768_CIPHERTEXTBYTES];
    uint8_t pq_ss[ML_KEM_768_SSBYTES];

    /* 1. 经典KEM封装 */
    size_t classic_ct_len;
    EVP_PKEY_encapsulate(hk->classic_kem, classic_ct, &classic_ct_len,
                         classic_ss, 32);

    /* 2. PQC KEM封装 */
    ml_kem_enc(pq_ct, pq_ss, hk->pq_kem);

    /* 3. 组合密文 */
    /* ct = len(classic_ct) || classic_ct || pq_ct */
    ct[0] = (classic_ct_len >> 8) & 0xFF;
    ct[1] = classic_ct_len & 0xFF;
    memcpy(ct + 2, classic_ct, classic_ct_len);
    memcpy(ct + 2 + classic_ct_len, pq_ct, ML_KEM_768_CIPHERTEXTBYTES);

    /* 4. 组合共享密钥 */
    /* ss = KDF(classic_ss || pq_ss) */
    uint8_t combined_ss[64];
    memcpy(combined_ss, classic_ss, 32);
    memcpy(combined_ss + 32, pq_ss, 32);

    /* 使用HKDF派生最终共享密钥 */
    hkdf_sha256(combined_ss, 64, NULL, 0,
                (uint8_t *)"hybrid-kem", 11, ss, 32);

    /* 清除中间值 */
    secure_memzero(classic_ss, sizeof(classic_ss));
    secure_memzero(pq_ss, sizeof(pq_ss));
    secure_memzero(combined_ss, sizeof(combined_ss));

    return 0;
}

/* 混合签名 */
typedef struct {
    EVP_PKEY *classic_key;      /* 经典签名密钥（如ECDSA） */
    ml_dsa_keypair_t pq_key;    /* PQC签名密钥（ML-DSA） */
} hybrid_sig_key_t;

typedef struct {
    uint8_t classic_sig[256];
    size_t classic_sig_len;
    ml_dsa_signature_t pq_sig;
} hybrid_signature_t;

int hybrid_sign(const uint8_t *msg, size_t msglen,
                hybrid_signature_t *sig,
                const hybrid_sig_key_t *hk) {
    /* 1. 经典签名 */
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, hk->classic_key);
    EVP_DigestSign(ctx, sig->classic_sig, &sig->classic_sig_len,
                   msg, msglen);
    EVP_MD_CTX_free(ctx);

    /* 2. PQC签名 */
    ml_dsa_sign(&sig->pq_sig, msg, msglen, &hk->pq_key, NULL, 0, NULL, 0);

    return 0;
}

int hybrid_verify(const uint8_t *msg, size_t msglen,
                  const hybrid_signature_t *sig,
                  const hybrid_sig_key_t *hk) {
    /* 1. 验证经典签名 */
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, hk->classic_key);
    int ret = EVP_DigestVerify(ctx, sig->classic_sig, sig->classic_sig_len,
                               msg, msglen);
    EVP_MD_CTX_free(ctx);

    if (ret != 1) {
        return -1;  /* 经典签名失败 */
    }

    /* 2. 验证PQC签名 */
    ret = ml_dsa_verify(msg, msglen, &sig->pq_sig,
                        hk->pq_key.pk, NULL, 0);

    if (ret != 0) {
        return -1;  /* PQC签名失败 */
    }

    return 0;
}

/* TLS 1.3混合密钥交换 */
#define TLS_GROUP_KYBER768X25519  0x6399

int tls_hybrid_key_exchange(EVP_PKEY **shared_key,
                            const uint8_t *client_key_exchange,
                            size_t ckex_len) {
    /* 分离X25519和ML-KEM-768组件 */
    uint16_t classic_len = (client_key_exchange[0] << 8) |
                           client_key_exchange[1];

    const uint8_t *classic_key = client_key_exchange + 2;
    const uint8_t *pq_key = client_key_exchange + 2 + classic_len;

    /* 分别处理两个组件 */
    /* ... */

    return 0;
}
