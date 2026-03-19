/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 809
 * Language: c
 * Block ID: 42555428
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PoW挖矿实现 */
typedef struct {
    uint32_t version;
    uint8_t prev_block[32];     /* 前一区块哈希 */
    uint8_t merkle_root[32];    /* Merkle根 */
    uint32_t timestamp;
    uint32_t bits;              /* 难度目标 */
    uint32_t nonce;             /* 随机数 */
} block_header_t;

/* 难度计算 - 根据目标值确定 */
uint32_t calculate_difficulty_target(uint32_t bits) {
    uint32_t exponent = (bits >> 24) & 0xFF;
    uint32_t coefficient = bits & 0x007FFFFF;

    /* target = coefficient * 256^(exponent-3) */
    /* 简化表示 */
    return bits;
}

/* PoW挖矿 */
bool pow_mine(block_header_t *header, uint32_t difficulty_bits) {
    uint8_t hash[32];
    uint32_t target = calculate_difficulty_target(difficulty_bits);

    for (uint32_t nonce = 0; nonce < UINT32_MAX; nonce++) {
        header->nonce = nonce;

        /* 双SHA256哈希 */
        sha256_ctx_t ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, header, sizeof(block_header_t));
        sha256_final(&ctx, hash);

        sha256_init(&ctx);
        sha256_update(&ctx, hash, 32);
        sha256_final(&ctx, hash);

        /* 检查是否满足难度目标（哈希小于目标） */
        if (memcmp(hash, &target, 4) < 0) {
            return true;  /* 找到有效nonce */
        }
    }

    return false;  /* 未找到 */
}

/* 难度调整 - 保持平均出块时间 */
uint32_t adjust_difficulty(uint32_t current_bits,
                           uint64_t actual_time_span,
                           uint64_t target_time_span) {
    /* Bitcoin: 每2016个区块调整一次 */
    uint64_t new_target = (uint64_t)current_bits * actual_time_span
                          / target_time_span;

    /* 限制调整幅度（4倍上限/下限） */
    if (new_target > current_bits * 4)
        new_target = current_bits * 4;
    if (new_target < current_bits / 4)
        new_target = current_bits / 4;

    return (uint32_t)new_target;
}
