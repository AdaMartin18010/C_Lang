/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\README.md
 * Line: 1313
 * Language: c
 * Block ID: 448449c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file voter.c
 * @brief 三模冗余 (TMR) 表决器实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CHANNEL_COUNT 3
#define CHANNEL_A 0
#define CHANNEL_B 1
#define CHANNEL_C 2

/**
 * @brief 通道状态
 */
typedef struct {
    bool healthy;           /* 健康状态 */
    uint32_t error_count;   /* 错误计数 */
    uint32_t last_valid_output;
} ChannelStatus;

/**
 * @brief 表决器状态
 */
typedef struct {
    ChannelStatus channels[CHANNEL_COUNT];
    uint32_t active_channels;
    uint32_t disagreement_threshold;
} VoterState;

/**
 * @brief 初始化表决器
 */
void voter_init(VoterState *voter) {
    memset(voter, 0, sizeof(VoterState));
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        voter->channels[i].healthy = true;
    }
    voter->active_channels = CHANNEL_COUNT;
    voter->disagreement_threshold = 10;
}

/**
 * @brief TMR 多数表决
 *
 * @param inputs 三个通道的输入
 * @param voter 表决器状态
 * @param output 表决输出
 * @return true 如果表决成功
 */
bool voter_majority_vote(const uint32_t inputs[CHANNEL_COUNT],
                         VoterState *voter,
                         uint32_t *output)
{
    uint32_t a = inputs[CHANNEL_A];
    uint32_t b = inputs[CHANNEL_B];
    uint32_t c = inputs[CHANNEL_C];

    /* 统计各通道健康状态 */
    bool valid[CHANNEL_COUNT];
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        valid[i] = voter->channels[i].healthy;
    }

    /* 多数表决逻辑 */
    if (valid[CHANNEL_A] && valid[CHANNEL_B] && valid[CHANNEL_C]) {
        /* 三个通道都健康 */
        if (a == b || a == c) {
            *output = a;
            voter->channels[CHANNEL_A].last_valid_output = a;

            if (a != b) {
                voter->channels[CHANNEL_B].error_count++;
            }
            if (a != c) {
                voter->channels[CHANNEL_C].error_count++;
            }
            return true;
        } else if (b == c) {
            *output = b;
            voter->channels[CHANNEL_A].error_count++;
            voter->channels[CHANNEL_B].last_valid_output = b;
            voter->channels[CHANNEL_C].last_valid_output = c;
            return true;
        } else {
            /* 三个通道输出都不同 - 严重故障 */
            return false;
        }
    } else if (valid[CHANNEL_A] && valid[CHANNEL_B]) {
        /* A和B健康 */
        *output = (a == b) ? a : a; /* 选择A（主通道） */
        return true;
    } else if (valid[CHANNEL_A] && valid[CHANNEL_C]) {
        /* A和C健康 */
        *output = (a == c) ? a : a;
        return true;
    } else if (valid[CHANNEL_B] && valid[CHANNEL_C]) {
        /* B和C健康 */
        *output = (b == c) ? b : b;
        return true;
    } else if (valid[CHANNEL_A]) {
        /* 只有A健康 */
        *output = a;
        return true;
    } else if (valid[CHANNEL_B]) {
        /* 只有B健康 */
        *output = b;
        return true;
    } else if (valid[CHANNEL_C]) {
        /* 只有C健康 */
        *output = c;
        return true;
    }

    /* 所有通道都故障 */
    return false;
}

/**
 * @brief 通道故障检测与隔离
 */
void voter_monitor_channels(VoterState *voter) {
    for (int i = 0; i < CHANNEL_COUNT; i++) {
        if (voter->channels[i].error_count > voter->disagreement_threshold) {
            printf("[VOTER] Channel %d isolated due to excessive errors\n", i);
            voter->channels[i].healthy = false;
            voter->active_channels--;
        }
    }

    if (voter->active_channels < 2) {
        printf("[VOTER] WARNING: System in degraded mode (%d active channels)\n",
               voter->active_channels);
    }
}

/* 演示 */
int main(void) {
    VoterState voter;
    voter_init(&voter);

    printf("=== TMR Voter Demo ===\n\n");

    /* 测试用例 */
    uint32_t test_cases[][3] = {
        {100, 100, 100},    /* 全部一致 */
        {100, 100, 101},    /* C有轻微偏差 */
        {100, 200, 100},    /* B有严重偏差 */
        {100, 200, 300},    /* 全部不一致 */
    };

    for (int i = 0; i < 4; i++) {
        uint32_t output;
        printf("Inputs: A=%d, B=%d, C=%d\n",
               test_cases[i][0], test_cases[i][1], test_cases[i][2]);

        if (voter_majority_vote(test_cases[i], &voter, &output)) {
            printf("  Output: %d\n", output);
        } else {
            printf("  VOTE FAILED!\n");
        }
        printf("  Error counts: A=%d, B=%d, C=%d\n\n",
               voter.channels[0].error_count,
               voter.channels[1].error_count,
               voter.channels[2].error_count);
    }

    return 0;
}
