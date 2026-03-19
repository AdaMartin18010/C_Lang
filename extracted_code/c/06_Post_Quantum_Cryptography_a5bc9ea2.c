/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 850
 * Language: c
 * Block ID: a5bc9ea2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ML-DSA (Module Lattice-based Digital Signature Algorithm)
 * NIST FIPS 204 标准实现
 *
 * 参数集：
 * - ML-DSA-44:  (d=13, τ=39, γ1=2^17, γ2=95232, k=4, l=4, η=2)
 * - ML-DSA-65:  (d=13, τ=49, γ1=2^19, γ2=261888, k=6, l=5, η=4)
 * - ML-DSA-87:  (d=13, τ=60, γ1=2^19, γ2=261888, k=8, l=7, η=2)
 */

/* ML-DSA-65 参数 */
#define ML_DSA_65_D         13
#define ML_DSA_65_TAU       49
#define ML_DSA_65_GAMMA1    (1 << 19)
#define ML_DSA_65_GAMMA2    261888
#define ML_DSA_65_K         6
#define ML_DSA_65_L         5
#define ML_DSA_65_ETA       4
#define ML_DSA_65_BETA      196
#define ML_DSA_65_OMEGA     192

/* 密钥大小 */
#define ML_DSA_65_PUBLICKEYBYTES    1952
#define ML_DSA_65_SECRETKEYBYTES    4032
#define ML_DSA_65_SIGBYTES          3293

/* 密钥对结构 */
typedef struct {
    uint8_t pk[ML_DSA_65_PUBLICKEYBYTES];
    uint8_t sk[ML_DSA_65_SECRETKEYBYTES];
} ml_dsa_keypair_t;

/* 签名结构 */
typedef struct {
    uint8_t sig[ML_DSA_65_SIGBYTES];
    size_t sig_len;
} ml_dsa_signature_t;

/* 密钥生成 */
int ml_dsa_keygen(ml_dsa_keypair_t *kp) {
    uint8_t seed[32];
    uint8_t rho[32], rho_prime[64], K[32];
    polyvec A[ML_DSA_65_K];
    polyvec s1, s2;
    polyvec t, t1, t0;

    /* 1. 生成随机种子 */
    randombytes(seed, 32);

    /* 2. 扩展种子 */
    expand_seed(seed, rho, rho_prime, K);

    /* 3. 从rho扩展矩阵A */
    expand_matrix_A(A, rho);

    /* 4. 采样秘密向量s1和s2 */
    expand_s1_s2(&s1, &s2, rho_prime);

    /* 5. 计算 t = A*s1 + s2 */
    for (int i = 0; i < ML_DSA_65_K; i++) {
        polyvec_basemul_acc_montgomery(&t.vec[i], &A[i], &s1);
        poly_invntt_tomont(&t.vec[i]);
        poly_add(&t.vec[i], &t.vec[i], &s2.vec[i]);
    }

    /* 6. 幂次舍入：t = 2^d * t1 + t0 */
    for (int i = 0; i < ML_DSA_65_K; i++) {
        poly_power2round(&t1.vec[i], &t0.vec[i], &t.vec[i]);
    }

    /* 7. 打包公钥 pk = (rho, t1) */
    pack_pk(kp->pk, rho, &t1);

    /* 8. 打包私钥 sk = (rho, K, tr, s1, s2, t0) */
    /* tr = CRH(rho || t1) */
    uint8_t tr[48];
    crh_tr(kp->pk, ML_DSA_65_PUBLICKEYBYTES, tr);

    pack_sk(kp->sk, rho, K, tr, &s1, &s2, &t0);

    return 0;
}

/* 签名 */
int ml_dsa_sign(ml_dsa_signature_t *sig,
                const uint8_t *m, size_t mlen,
                const uint8_t *sk,
                const uint8_t *ctx, size_t ctxlen) {
    polyvec A[ML_DSA_65_K];
    polyvec s1, s2, t0;
    uint8_t rho[32], K[32], tr[48];
    uint8_t mu[64];
    polyvec y, w, w1;
    polyvec z, h;
    poly c, cs1, cs2, ct0;
    uint8_t seed[32];

    /* 1. 解包私钥 */
    unpack_sk(rho, K, tr, &s1, &s2, &t0, sk);

    /* 2. 计算 mu = CRH(tr || ctx || m) */
    compute_mu(mu, tr, ctx, ctxlen, m, mlen);

    /* 3. 扩展矩阵A */
    expand_matrix_A(A, rho);

    /* 4. 签名循环（拒绝采样） */
    for (int attempt = 0; attempt < 1000; attempt++) {
        /* 采样掩码向量y */
        expand_mask(&y, K, mu, attempt);

        /* 计算 w = A*y */
        for (int i = 0; i < ML_DSA_65_K; i++) {
            polyvec_basemul_acc_montgomery(&w.vec[i], &A[i], &y);
            poly_invntt_tomont(&w.vec[i]);
        }

        /* 计算w1 = high_bits(w, 2*γ2) */
        for (int i = 0; i < ML_DSA_65_K; i++) {
            poly_highbits(&w1.vec[i], &w.vec[i], 2 * ML_DSA_65_GAMMA2);
        }

        /* 计算挑战c = H(mu || w1) */
        uint8_t c_tilde[32];
        challenge(c_tilde, mu, &w1);
        poly_challenge(&c, c_tilde);

        /* 计算z = y + c*s1 */
        polyvec_copy(&z, &y);
        for (int i = 0; i < ML_DSA_65_L; i++) {
            poly_ntt(&c);
            poly_pointwise_montgomery(&cs1, &c, &s1.vec[i]);
            poly_invntt_tomont(&cs1);
            poly_add(&z.vec[i], &z.vec[i], &cs1);
        }

        /* 检查z的范数 */
        if (polyvec_chknorm(&z, ML_DSA_65_GAMMA1 - ML_DSA_65_BETA)) {
            continue;  /* 拒绝 */
        }

        /* 计算 hints */
        /* ... */

        /* 打包签名 */
        pack_sig(sig->sig, c_tilde, &z, &h);
        sig->sig_len = ML_DSA_65_SIGBYTES;

        return 0;
    }

    return -1;  /* 签名失败 */
}

/* 验签 */
int ml_dsa_verify(const uint8_t *m, size_t mlen,
                  const ml_dsa_signature_t *sig,
                  const uint8_t *pk,
                  const uint8_t *ctx, size_t ctxlen) {
    polyvec A[ML_DSA_65_K], t1;
    polyvec z, h, w1_approx;
    poly c;
    uint8_t rho[32], mu[64];
    uint8_t c_tilde[32], c_tilde_prime[32];

    /* 1. 解包公钥 */
    unpack_pk(rho, &t1, pk);

    /* 2. 计算 mu = CRH(CRH(rho || t1) || ctx || m) */
    uint8_t tr[48];
    crh_tr(pk, ML_DSA_65_PUBLICKEYBYTES, tr);
    compute_mu(mu, tr, ctx, ctxlen, m, mlen);

    /* 3. 解包签名 */
    if (unpack_sig(c_tilde, &z, &h, sig->sig, sig->sig_len) != 0) {
        return -1;  /* 签名格式无效 */
    }

    /* 4. 检查z的范数 */
    if (polyvec_chknorm(&z, ML_DSA_65_GAMMA1 - ML_DSA_65_BETA)) {
        return -1;  /* 范数过大 */
    }

    /* 5. 重建挑战 */
    poly_challenge(&c, c_tilde);

    /* 6. 扩展矩阵A */
    expand_matrix_A(A, rho);

    /* 7. 计算 w' = A*z - c*t1*2^d */
    for (int i = 0; i < ML_DSA_65_K; i++) {
        polyvec_basemul_acc_montgomery(&w1_approx.vec[i], &A[i], &z);
        /* ... 减去c*t1*2^d ... */
        poly_invntt_tomont(&w1_approx.vec[i]);
    }

    /* 8. 使用hints重建w1 */
    use_hints(&w1_approx, &h, &t1);

    /* 9. 计算 c' = H(mu || w1') */
    challenge(c_tilde_prime, mu, &w1_approx);

    /* 10. 比较挑战 */
    if (memcmp(c_tilde, c_tilde_prime, 32) != 0) {
        return -1;  /* 验证失败 */
    }

    return 0;
}
