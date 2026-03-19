/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_Radiation_Hardening.md
 * Line: 172
 * Language: c
 * Block ID: b93f1cb0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 三模冗余投票
typedef struct {
    uint32_t value_a;
    uint32_t value_b;
    uint32_t value_c;
    uint32_t voted;     // 上次投票结果
    uint8_t mismatch;   // 不匹配计数
} TMRVariable;

// 多数投票
static inline uint32_t majority_vote(uint32_t a, uint32_t b, uint32_t c) {
    // 如果至少两个相等，返回该值
    if (a == b) return a;
    if (a == c) return a;
    if (b == c) return b;

    // 全不同 - 使用上次投票值（或特殊处理）
    return 0;  // 或触发错误处理
}

uint32_t tmr_read(TMRVariable *tmr) {
    uint32_t voted = majority_vote(tmr->value_a, tmr->value_b, tmr->value_c);

    // 检查不匹配
    int matches = (tmr->value_a == voted) +
                  (tmr->value_b == voted) +
                  (tmr->value_c == voted);

    if (matches < 3) {
        tmr->mismatch++;

        // 自动纠正错误副本
        if (tmr->value_a != voted) tmr->value_a = voted;
        if (tmr->value_b != voted) tmr->value_b = voted;
        if (tmr->value_c != voted) tmr->value_c = voted;
    }

    tmr->voted = voted;
    return voted;
}

void tmr_write(TMRVariable *tmr, uint32_t value) {
    tmr->value_a = value;
    tmr->value_b = value;
    tmr->value_c = value;
    tmr->voted = value;
}

// TMR寄存器组
typedef struct {
    TMRVariable status;
    TMRVariable control;
    TMRVariable data[8];
} TMRRegisterBank;
