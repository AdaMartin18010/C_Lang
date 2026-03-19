/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\README.md
 * Line: 636
 * Language: c
 * Block ID: 45740e7e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 常量时间比较 - 防止时序攻击 */
int constant_time_memcmp(const uint8_t *a,
                         const uint8_t *b,
                         size_t len) {
    volatile uint8_t result = 0;

    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];  /* XOR累积，无分支 */
    }

    return result;  /* 0表示相等，非0表示不等 */
}

/* 常量时间条件选择 */
uint32_t constant_time_select(uint32_t mask,    /* 全0或全1 */
                              uint32_t a,       /* mask=1时选择 */
                              uint32_t b) {     /* mask=0时选择 */
    return (mask & a) | (~mask & b);
}

/* 常量时间幂运算 (RSA/ECC) */
void constant_time_expmod(const bignum_t *base,
                          const bignum_t *exp,
                          const bignum_t *mod,
                          bignum_t *result) {
    /* Montgomery ladder - 每比特固定操作 */
    bignum_t r0 = 1, r1 = base;

    for (int i = exp->bitlen - 1; i >= 0; i--) {
        uint32_t bit = (exp->words[i / 32] >> (i % 32)) & 1;
        uint32_t mask = 0 - bit;  /* 0或0xFFFFFFFF */

        /* 始终进行两次乘法，根据比特选择结果 */
        bignum_t t0 = montgomery_mult(r0, r0, mod);
        bignum_t t1 = montgomery_mult(r0, r1, mod);

        r0 = constant_time_select(mask, t1, t0);
        r1 = constant_time_select(mask,
            montgomery_mult(r1, r1, mod), t1);
    }

    *result = r0;
}

/* 缓存攻击防护 - 内存访问模式隐藏 */
void cache_safe_aes_sbox(uint8_t *state) {
    /* 使用bit-slicing或查找表随机化 */
    static const uint8_t sbox[256] = { /* AES S-box */ };

    /* 方法1: 预加载整个表到缓存 */
    volatile uint8_t dummy = 0;
    for (int i = 0; i < 256; i++) {
        dummy |= sbox[i];  /* 访问所有条目 */
    }

    /* 方法2: 使用掩码避免直接索引 */
    for (int i = 0; i < 16; i++) {
        uint8_t idx = state[i];
        /* 通过线性扫描隐藏访问模式 */
        uint8_t result = 0;
        for (int j = 0; j < 256; j++) {
            uint8_t mask = constant_time_eq(j, idx);
            result |= mask & sbox[j];
        }
        state[i] = result;
    }
}
