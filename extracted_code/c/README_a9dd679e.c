/*
 * Auto-generated from: 03_System_Technology_Domains\08_Distributed_Consensus\README.md
 * Line: 877
 * Language: c
 * Block ID: a9dd679e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PoS区块提议选择算法 */

/* 验证者信息 */
typedef struct {
    uint8_t validator_id[32];   /* 验证者公钥/地址 */
    uint64_t stake;              /* 质押金额 */
    uint64_t activation_epoch;   /* 激活纪元 */
} validator_t;

/* 基于VRF的随机选择 */
typedef struct {
    uint8_t proof[64];          /* VRF证明 */
    uint8_t hash[32];           /* VRF输出哈希 */
    uint64_t slot;               /* 时隙号 */
} vrf_output_t;

/* 计算验证者被选中的概率权重 */
uint64_t calculate_validator_weight(const validator_t *validator,
                                    uint64_t total_stake,
                                    uint64_t epoch) {
    if (epoch < validator->activation_epoch)
        return 0;  /* 尚未激活 */

    /* 权重与质押成正比，但可能有上限防止中心化 */
    uint64_t weight = validator->stake;
    uint64_t max_effective_stake = total_stake / 100;  /* 例如1%上限 */

    if (weight > max_effective_stake)
        weight = max_effective_stake;

    return weight;
}

/* 选择下一个区块提议者 */
int select_proposer(validator_t *validators, int validator_count,
                    uint64_t total_stake, uint64_t epoch, uint64_t slot,
                    const uint8_t *randao_mix) {
    /* 使用RANDAO + 槽位作为种子 */
    uint8_t seed[64];
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, randao_mix, 32);
    sha256_update(&ctx, &slot, sizeof(slot));
    sha256_final(&ctx, seed);

    /* 生成随机数 [0, total_stake) */
    uint64_t rand_value = (*(uint64_t*)seed) % total_stake;

    /* 按权重选择 */
    uint64_t cumulative = 0;
    for (int i = 0; i < validator_count; i++) {
        cumulative += calculate_validator_weight(&validators[i],
                                                  total_stake, epoch);
        if (rand_value < cumulative)
            return i;  /* 选中此验证者 */
    }

    return validator_count - 1;  /* 默认选最后一个 */
}

/* Casper FFG 最终性检查点 */
typedef struct {
    uint64_t epoch;
    uint8_t checkpoint_hash[32];
    uint64_t justified_stake;    /* 投赞成票的质押量 */
    uint64_t total_stake;
} checkpoint_t;

bool is_justified(const checkpoint_t *cp, uint64_t threshold_numerator,
                  uint64_t threshold_denominator) {
    /* 需要 2/3 质押量投票 */
    return cp->justified_stake * threshold_denominator >=
           cp->total_stake * threshold_numerator;
}

bool is_finalized(const checkpoint_t *current,
                  const checkpoint_t *previous) {
    /* 当前检查点已确认且父检查点也已确认 */
    return current->justified && previous->justified &&
           memcmp(current->parent_hash, previous->checkpoint_hash, 32) == 0;
}
