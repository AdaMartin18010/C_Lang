/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\02_TMR_Voting.md
 * Line: 101
 * Language: c
 * Block ID: 81edb498
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// TMR表决器实现
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// 表决结果
typedef enum {
    VOTE_AGREE = 0,         // 三模块一致
    VOTE_CORRECTED = 1,     // 两模块一致，第三模块故障
    VOTE_DISAGREE = -1,     // 三模块均不一致
    VOTE_MASKED = -2        // 故障被屏蔽
} VoteResult;

// 模块状态
typedef enum {
    MODULE_HEALTHY = 0,
    MODULE_SUSPECT = 1,
    MODULE_FAILED = 2
} ModuleStatus;

// TMR通道 (单个变量)
typedef struct {
    uint32_t value_a;
    uint32_t value_b;
    uint32_t value_c;
    uint32_t voted_value;
    VoteResult result;
    ModuleStatus status_a;
    ModuleStatus status_b;
    ModuleStatus status_c;
    uint32_t mismatch_count_a;
    uint32_t mismatch_count_b;
    uint32_t mismatch_count_c;
} TMRChannel32;

// 初始化TMR通道
void tmr_channel_init(TMRChannel32 *tmr) {
    tmr->value_a = 0;
    tmr->value_b = 0;
    tmr->value_c = 0;
    tmr->voted_value = 0;
    tmr->result = VOTE_AGREE;
    tmr->status_a = MODULE_HEALTHY;
    tmr->status_b = MODULE_HEALTHY;
    tmr->status_c = MODULE_HEALTHY;
    tmr->mismatch_count_a = 0;
    tmr->mismatch_count_b = 0;
    tmr->mismatch_count_c = 0;
}

// 多数表决 (32位)
VoteResult majority_vote_32(TMRChannel32 *tmr) {
    bool ab_match = (tmr->value_a == tmr->value_b);
    bool bc_match = (tmr->value_b == tmr->value_c);
    bool ac_match = (tmr->value_a == tmr->value_c);

    if (ab_match && bc_match) {
        // A = B = C
        tmr->voted_value = tmr->value_a;
        tmr->result = VOTE_AGREE;

        // 重置不匹配计数
        tmr->mismatch_count_a = 0;
        tmr->mismatch_count_b = 0;
        tmr->mismatch_count_c = 0;

        return VOTE_AGREE;
    }

    if (ab_match) {
        // A = B ≠ C
        tmr->voted_value = tmr->value_a;
        tmr->result = VOTE_CORRECTED;
        tmr->mismatch_count_c++;

        if (tmr->mismatch_count_c >= 3) {
            tmr->status_c = MODULE_FAILED;
        } else {
            tmr->status_c = MODULE_SUSPECT;
        }

        return VOTE_CORRECTED;
    }

    if (bc_match) {
        // B = C ≠ A
        tmr->voted_value = tmr->value_b;
        tmr->result = VOTE_CORRECTED;
        tmr->mismatch_count_a++;

        if (tmr->mismatch_count_a >= 3) {
            tmr->status_a = MODULE_FAILED;
        } else {
            tmr->status_a = MODULE_SUSPECT;
        }

        return VOTE_CORRECTED;
    }

    if (ac_match) {
        // A = C ≠ B
        tmr->voted_value = tmr->value_a;
        tmr->result = VOTE_CORRECTED;
        tmr->mismatch_count_b++;

        if (tmr->mismatch_count_b >= 3) {
            tmr->status_b = MODULE_FAILED;
        } else {
            tmr->status_b = MODULE_SUSPECT;
        }

        return VOTE_CORRECTED;
    }

    // 三者都不匹配
    tmr->result = VOTE_DISAGREE;

    // 选择"最健康"的模块
    if (tmr->status_a == MODULE_HEALTHY) {
        tmr->voted_value = tmr->value_a;
    } else if (tmr->status_b == MODULE_HEALTHY) {
        tmr->voted_value = tmr->value_b;
    } else if (tmr->status_c == MODULE_HEALTHY) {
        tmr->voted_value = tmr->value_c;
    } else {
        // 全部故障，选择历史最可靠的
        // 简化: 选择A
        tmr->voted_value = tmr->value_a;
    }

    return VOTE_DISAGREE;
}

// 写入TMR值 (来自各模块)
void tmr_write_values(TMRChannel32 *tmr, uint32_t val_a,
                      uint32_t val_b, uint32_t val_c) {
    tmr->value_a = val_a;
    tmr->value_b = val_b;
    tmr->value_c = val_c;

    majority_vote_32(tmr);
}

// 获取表决值
uint32_t tmr_read_voted(const TMRChannel32 *tmr) {
    return tmr->voted_value;
}

// 检查是否有模块故障
bool tmr_has_failure(const TMRChannel32 *tmr) {
    return (tmr->status_a == MODULE_FAILED ||
            tmr->status_b == MODULE_FAILED ||
            tmr->status_c == MODULE_FAILED);
}
