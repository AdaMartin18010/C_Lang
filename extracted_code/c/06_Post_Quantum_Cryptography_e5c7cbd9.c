/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 1068
 * Language: c
 * Block ID: e5c7cbd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SLH-DSA (Stateless Hash-Based Digital Signature Algorithm)
 * NIST FIPS 205 标准实现
 *
 * SPHINCS+ 使用Hypertree结构（XMSS/XMSSMT）和FORS签名
 *
 * 参数集（SHA2-128s）：
 * - n=16, h=66, d=22, a=6, k=33, w=16
 */

#include <string.h>

/* SLH-DSA-SHA2-128s 参数 */
#define SLH_DSA_N           16      /* 安全参数 */
#define SLH_DSA_H           66      /* 树高 */
#define SLH_DSA_D           22      /* 树层数 */
#define SLH_DSA_A           6       /* FORS树高 */
#define SLH_DSA_K           33      /* FORS树数量 */
#define SLH_DSA_W           16      /* Winternitz参数 */

/* 派生参数 */
#define SLH_DSA_H_PRIME     (SLH_DSA_H / SLH_DSA_D)  /* XMSS树高 */
#define SLH_DSA_T           (1 << SLH_DSA_A)         /* FORS叶子数 */

/* 密钥大小 */
#define SLH_DSA_SECRETKEYBYTES  64
#define SLH_DSA_PUBLICKEYBYTES  32
#define SLH_DSA_SIGBYTES        7856

/* 密钥对结构 */
typedef struct {
    uint8_t pk[SLH_DSA_PUBLICKEYBYTES];
    uint8_t sk[SLH_DSA_SECRETKEYBYTES];
} slh_dsa_keypair_t;

/* 地址类型 */
typedef struct {
    uint32_t layer;
    uint64_t tree;
    uint32_t type;
    uint32_t keypair;
    uint32_t chain;
    uint32_t hash;
} slh_address_t;

/* FORS签名 */
typedef struct {
    uint8_t sig[SLH_DSA_K * (SLH_DSA_A + 1) * SLH_DSA_N];
} fors_signature_t;

/* HT（Hypertree）签名 */
typedef struct {
    uint8_t sig[SLH_DSA_D * (1 + SLH_DSA_H_PRIME) * SLH_DSA_N];
} ht_signature_t;

/* 完整签名 */
typedef struct {
    uint8_t randomness[SLH_DSA_N];
    fors_signature_t fors_sig;
    ht_signature_t ht_sig;
} slh_dsa_signature_t;

/* 密钥生成 */
int slh_dsa_keygen(slh_dsa_keypair_t *kp) {
    uint8_t seed[96];  /* SK.seed (32) || SK.prf (32) || PK.seed (32) */

    /* 生成随机种子 */
    randombytes(seed, sizeof(seed));

    /* 分离种子 */
    uint8_t *sk_seed = seed;
    uint8_t *sk_prf = seed + 32;
    uint8_t *pk_seed = seed + 64;

    /* 复制到私钥 */
    memcpy(kp->sk, sk_seed, 32);
    memcpy(kp->sk + 32, sk_prf, 32);

    /* 复制PK.seed到公钥和私钥 */
    memcpy(kp->pk, pk_seed, 32);
    memcpy(kp->sk + 64, pk_seed, 32);  /* PK.seed在私钥末尾 */

    /* 计算PK.root */
    slh_address_t addr = {0};
    addr.type = ADDR_TYPE_WOTS_PK;

    /* 生成最顶层XMSS树的根 */
    uint8_t pk_root[SLH_DSA_N];
    whtree_root(pk_root, sk_seed, pk_seed, &addr);

    memcpy(kp->pk + 32, pk_root, SLH_DSA_N);
    memcpy(kp->sk + 32 + 32 + 32, pk_root, SLH_DSA_N);

    return 0;
}

/* FORS签名 */
void fors_sign(fors_signature_t *sig,
               uint8_t *pk,
               const uint8_t *msg,
               const uint8_t *sk_seed,
               const uint8_t *pk_seed,
               slh_address_t *addr) {
    uint32_t indices[SLH_DSA_K];

    /* 1. 计算索引 */
    base_2b(msg, SLH_DSA_A, SLH_DSA_K, indices);

    /* 2. 对每个FORS树签名 */
    for (uint32_t i = 0; i < SLH_DSA_K; i++) {
        addr->type = ADDR_TYPE_FORS_TREE;
        addr->keypair = i;

        /* 签名路径 */
        uint8_t *sig_ptr = sig->sig + i * (SLH_DSA_A + 1) * SLH_DSA_N;

        /* 叶子节点 */
        addr->type = ADDR_TYPE_FORS_TREE;
        addr->tree_height = 0;
        addr->tree_index = indices[i];

        fors_gen_sk(sig_ptr, sk_seed, pk_seed, addr);
        sig_ptr += SLH_DSA_N;

        /* 认证路径 */
        uint8_t root[SLH_DSA_N];
        fors_treehash(root, sig_ptr, sk_seed, pk_seed,
                      addr->keypair, indices[i], sk_seed);
    }

    /* 3. 计算FORS公钥 */
    fors_pk_from_sig(pk, sig, msg, pk_seed, addr);
}

/* 签名 */
int slh_dsa_sign(uint8_t *sig, size_t *sig_len,
                 const uint8_t *m, size_t mlen,
                 const slh_dsa_keypair_t *kp,
                 const uint8_t *ctx, size_t ctxlen,
                 const uint8_t *ph_m, size_t ph_mlen) {
    slh_dsa_signature_t *s = (slh_dsa_signature_t *)sig;

    /* 分离种子 */
    uint8_t *sk_seed = kp->sk;
    uint8_t *sk_prf = kp->sk + 32;
    uint8_t *pk_seed = kp->sk + 64;
    uint8_t *pk_root = kp->sk + 96;

    /* 1. 生成随机数 */
    uint8_t opt_rand[SLH_DSA_N];
    memcpy(opt_rand, pk_seed, SLH_DSA_N);

    /* R = PRF_msg(SK.prf, opt_rand, M) */
    uint8_t m_digest[64];
    hash_msg(m_digest, m, mlen, ctx, ctxlen);

    prf_msg(s->randomness, sk_prf, opt_rand, m_digest, 64);

    /* 2. 计算消息摘要 */
    uint8_t digest[SLH_DSA_H / 8 + SLH_DSA_A * SLH_DSA_K / 8 + SLH_DSA_N];
    h_msg(digest, m_digest, 64, pk_seed, s->randomness, sizeof(digest));

    uint8_t *md = digest;
    uint8_t *idx_tree = digest + SLH_DSA_H / 8;
    uint8_t *idx_leaf = digest + SLH_DSA_H / 8 + 8;

    /* 3. FORS签名 */
    slh_address_t addr = {0};
    addr.layer = SLH_DSA_D;
    addr.tree = 0;
    for (int i = 0; i < 8; i++) {
        addr.tree = (addr.tree << 8) | idx_tree[i];
    }

    uint32_t leaf_idx = ((uint32_t)idx_leaf[0] << 24) |
                        ((uint32_t)idx_leaf[1] << 16) |
                        ((uint32_t)idx_leaf[2] << 8) |
                        idx_leaf[3];
    leaf_idx &= ((1 << SLH_DSA_H_PRIME) - 1);

    addr.type = ADDR_TYPE_WOTS_PK;
    addr.keypair = leaf_idx;

    uint8_t fors_pk[SLH_DSA_N];
    fors_sign(&s->fors_sig, fors_pk, md, sk_seed, pk_seed, &addr);

    /* 4. HT签名 */
    ht_sign((uint8_t *)&s->ht_sig, fors_pk, SLH_DSA_D,
            sk_seed, pk_seed, idx_tree, leaf_idx);

    *sig_len = SLH_DSA_SIGBYTES;
    return 0;
}

/* 验签 */
int slh_dsa_verify(const uint8_t *m, size_t mlen,
                   const uint8_t *sig, size_t sig_len,
                   const uint8_t *pk,
                   const uint8_t *ctx, size_t ctxlen) {
    if (sig_len != SLH_DSA_SIGBYTES) {
        return -1;
    }

    slh_dsa_signature_t *s = (slh_dsa_signature_t *)sig;

    uint8_t *pk_seed = (uint8_t *)pk;
    uint8_t *pk_root = (uint8_t *)pk + 32;

    /* 1. 计算消息摘要 */
    uint8_t m_digest[64];
    hash_msg(m_digest, m, mlen, ctx, ctxlen);

    uint8_t digest[SLH_DSA_H / 8 + SLH_DSA_A * SLH_DSA_K / 8 + SLH_DSA_N];
    h_msg(digest, m_digest, 64, pk_seed, s->randomness, sizeof(digest));

    uint8_t *md = digest;
    uint8_t *idx_tree = digest + SLH_DSA_H / 8;
    uint8_t *idx_leaf = digest + SLH_DSA_H / 8 + 8;

    /* 2. 从FORS签名恢复PK */
    slh_address_t addr = {0};
    addr.layer = SLH_DSA_D;
    addr.tree = 0;
    for (int i = 0; i < 8; i++) {
        addr.tree = (addr.tree << 8) | idx_tree[i];
    }

    uint32_t leaf_idx = ((uint32_t)idx_leaf[0] << 24) |
                        ((uint32_t)idx_leaf[1] << 16) |
                        ((uint32_t)idx_leaf[2] << 8) |
                        idx_leaf[3];
    leaf_idx &= ((1 << SLH_DSA_H_PRIME) - 1);

    uint8_t fors_pk[SLH_DSA_N];
    fors_pk_from_sig(fors_pk, &s->fors_sig, md, pk_seed, &addr);

    /* 3. 验证HT签名 */
    uint8_t ht_pk[SLH_DSA_N];
    ht_verify(ht_pk, (uint8_t *)&s->ht_sig, fors_pk,
              SLH_DSA_D, pk_seed, idx_tree, leaf_idx);

    /* 4. 比较根 */
    if (memcmp(ht_pk, pk_root, SLH_DSA_N) != 0) {
        return -1;  /* 验证失败 */
    }

    return 0;
}
