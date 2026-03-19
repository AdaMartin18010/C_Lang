/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 628
 * Language: c
 * Block ID: 846214de
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ML-KEM (Module Lattice-based Key Encapsulation Mechanism)
 * NIST FIPS 203 标准实现
 *
 * 参数集：
 * - ML-KEM-512:  k=2, η1=3, d_u=10, d_v=4
 * - ML-KEM-768:  k=3, η1=2, d_u=10, d_v=4
 * - ML-KEM-1024: k=4, η1=2, d_u=11, d_v=5
 */

#include <string.h>

/* ML-KEM-768 参数 */
#define ML_KEM_768_K        3
#define ML_KEM_768_ETA1     2
#define ML_KEM_768_DU       10
#define ML_KEM_768_DV       4

/* 密钥大小 */
#define ML_KEM_768_SECRETKEYBYTES   2400
#define ML_KEM_768_PUBLICKEYBYTES   1184
#define ML_KEM_768_CIPHERTEXTBYTES  1088
#define ML_KEM_768_SSBYTES          32

/* 密钥对结构 */
typedef struct {
    uint8_t pk[ML_KEM_768_PUBLICKEYBYTES];
    uint8_t sk[ML_KEM_768_SECRETKEYBYTES];
} ml_kem_keypair_t;

/* 密钥生成 */
int ml_kem_keygen(ml_kem_keypair_t *kp) {
    uint8_t d[ML_KEM_768_SSBYTES];
    uint8_t z[ML_KEM_768_SSBYTES];

    /* 1. 生成随机种子 */
    randombytes(d, sizeof(d));
    randombytes(z, sizeof(z));

    /* 2. 生成公钥和私钥 */
    /* 内部调用G函数扩展种子 */
    uint8_t expanded[64];
    hash_g(d, sizeof(d), expanded);

    uint8_t *rho = expanded;        /* 用于生成A矩阵 */
    uint8_t *sigma = expanded + 32; /* 用于采样秘密向量 */

    /* 3. 生成公钥 pk = (rho, t) */
    polyvec a[ML_KEM_768_K];        /* A矩阵 */
    polyvec s, e;                   /* 秘密向量和误差向量 */
    polyvec t;                      /* t = A*s + e */

    /* 从rho扩展A矩阵 */
    gen_matrix(a, rho, 0);

    /* 采样秘密向量s和误差向量e */
    for (int i = 0; i < ML_KEM_768_K; i++) {
        poly_getnoise(&s.vec[i], sigma, i);
        poly_getnoise(&e.vec[i], sigma, i + ML_KEM_768_K);
    }

    /* NTT转换 */
    polyvec_ntt(&s);

    /* 计算 t = A*s + e */
    for (int i = 0; i < ML_KEM_768_K; i++) {
        polyvec_basemul_acc_montgomery(&t.vec[i], &a[i], &s);
        poly_invntt_tomont(&t.vec[i]);
        poly_add(&t.vec[i], &t.vec[i], &e.vec[i]);
    }

    /* 打包公钥 */
    pack_pk(kp->pk, &t, rho);

    /* 打包私钥 */
    pack_sk(kp->sk, &s);

    /* 附加信息到私钥 */
    memcpy(kp->sk + ML_KEM_768_SECRETKEYBYTES - 2 * ML_KEM_768_SSBYTES,
           kp->pk, ML_KEM_768_PUBLICKEYBYTES);
    memcpy(kp->sk + ML_KEM_768_SECRETKEYBYTES - ML_KEM_768_SSBYTES,
           z, ML_KEM_768_SSBYTES);
    kp->sk[ML_KEM_768_SECRETKEYBYTES - ML_KEM_768_SSBYTES - 1] = ML_KEM_768_K;

    return 0;
}

/* 封装 */
int ml_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk) {
    polyvec t, r, e1;
    poly e2, v, mp;
    uint8_t seed[ML_KEM_768_SSBYTES];
    uint8_t m[ML_KEM_768_SSBYTES];
    uint8_t rho[32];

    /* 1. 生成随机消息m */
    randombytes(m, sizeof(m));

    /* 2. 计算H(m)和G(H(pk), m) */
    uint8_t h_pk[32];
    hash_h(pk, ML_KEM_768_PUBLICKEYBYTES, h_pk);

    uint8_t g_input[64];
    memcpy(g_input, h_pk, 32);
    memcpy(g_input + 32, m, 32);

    uint8_t g_output[64];
    hash_g(g_input, 64, g_output);
    memcpy(seed, g_output, 32);     /* K^ */
    memcpy(m, g_output + 32, 32);   /* 使用的随机性 */

    /* 3. 解包公钥 */
    unpack_pk(&t, rho, pk);
    polyvec_ntt(&t);

    /* 4. 从m采样r, e1, e2 */
    for (int i = 0; i < ML_KEM_768_K; i++) {
        poly_getnoise_eta1(&r.vec[i], m, i);
        poly_getnoise_eta2(&e1.vec[i], m, i + ML_KEM_768_K);
    }
    poly_getnoise_eta2(&e2, m, 2 * ML_KEM_768_K);

    /* 5. 转换为NTT域 */
    polyvec_ntt(&r);

    /* 6. 计算 u = A^T * r + e1 */
    polyvec a[ML_KEM_768_K];
    gen_matrix(a, rho, 1);

    for (int i = 0; i < ML_KEM_768_K; i++) {
        polyvec_basemul_acc_montgomery(&e1.vec[i], &a[i], &r);
        poly_invntt_tomont(&e1.vec[i]);
    }

    /* 7. 计算 v = t^T * r + e2 + Decompress(Decode(m), 1) */
    polyvec_basemul_acc_montgomery(&mp, &t, &r);
    poly_invntt_tomont(&mp);
    poly_add(&mp, &mp, &e2);

    poly msg_poly;
    poly_frommsg(&msg_poly, m);
    poly_add(&mp, &mp, &msg_poly);

    /* 8. 压缩并打包密文 */
    polyvec_compress_du(ct, &e1);
    poly_compress_dv(ct + ML_KEM_768_K * ML_KEM_N * ML_KEM_768_DU / 8, &mp);

    /* 9. 派生共享密钥 ss = KDF(K^ || H(ct)) */
    uint8_t h_ct[32];
    hash_h(ct, ML_KEM_768_CIPHERTEXTBYTES, h_ct);

    uint8_t kdf_input[64];
    memcpy(kdf_input, seed, 32);
    memcpy(kdf_input + 32, h_ct, 32);
    hash_j(kdf_input, 64, ss);

    return 0;
}

/* 解封装 */
int ml_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk) {
    polyvec s, u;
    poly mp, v, tmp;
    uint8_t rho[32];
    uint8_t m[ML_KEM_768_SSBYTES];
    uint8_t z[ML_KEM_768_SSBYTES];

    /* 1. 解包私钥 */
    unpack_sk(&s, sk);

    /* 2. 解包密文 */
    polyvec_decompress_du(&u, ct);
    poly_decompress_dv(&v, ct + ML_KEM_768_K * ML_KEM_N * ML_KEM_768_DU / 8);

    /* 3. 计算 m' = v - s^T * u */
    polyvec_ntt(&u);
    polyvec_basemul_acc_montgomery(&tmp, &s, &u);
    poly_invntt_tomont(&tmp);
    poly_sub(&mp, &v, &tmp);

    /* 4. 提取消息 */
    poly_tomsg(m, &mp);

    /* 5. 重新计算密钥以验证 */
    /* 加载公钥哈希和z */
    memcpy(rho, sk + ML_KEM_768_SECRETKEYBYTES - 2 * ML_KEM_768_SSBYTES + ML_KEM_768_PUBLICKEYBYTES - 32, 32);
    memcpy(z, sk + ML_KEM_768_SECRETKEYBYTES - ML_KEM_768_SSBYTES, 32);

    /* 重新封装验证 */
    uint8_t ct_verify[ML_KEM_768_CIPHERTEXTBYTES];
    uint8_t ss_verify[ML_KEM_768_SSBYTES];

    /* 使用临时密钥进行验证封装 */
    uint8_t pk[ML_KEM_768_PUBLICKEYBYTES];
    memcpy(pk, sk + ML_KEM_768_SECRETKEYBYTES - 2 * ML_KEM_768_SSBYTES - ML_KEM_768_PUBLICKEYBYTES,
           ML_KEM_768_PUBLICKEYBYTES);

    /* 模拟封装过程 */
    ml_kem_enc(ct_verify, ss_verify, pk);

    /* 6. 验证密文 */
    uint8_t fail = verify_constant_time(ct, ct_verify, ML_KEM_768_CIPHERTEXTBYTES);

    /* 7. 派生共享密钥 */
    uint8_t h_ct[32];
    hash_h(ct, ML_KEM_768_CIPHERTEXTBYTES, h_ct);

    /* 如果验证失败，使用z代替（隐式拒绝） */
    cmov(m, z, ML_KEM_768_SSBYTES, fail);

    uint8_t g_input[64];
    memcpy(g_input, m, 32);
    memcpy(g_input + 32, h_ct, 32);
    hash_j(g_input, 64, ss);

    return 0;
}
