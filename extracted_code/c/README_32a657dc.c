/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\README.md
 * Line: 476
 * Language: c
 * Block ID: 32a657dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* priority_matrix.h */
#ifndef PRIORITY_MATRIX_H
#define PRIORITY_MATRIX_H

#include <stdint.h>

/* 优先级象限 */
typedef enum {
    QUADRANT_QUICK_WINS = 0,    /* 高影响低难度 - 快速收益 */
    QUADRANT_MAJOR_PROJECTS,    /* 高影响高难度 - 重大项目 */
    QUADRANT_FILL_INS,          /* 低影响低难度 - 填充时间 */
    QUADRANT_THANKLESS,         /* 低影响高难度 - 吃力不讨好 */
    QUADRANT_COUNT
} priority_quadrant_t;

/* 学习项评估 */
typedef struct {
    uint32_t skill_id;
    float impact;           /* 影响力 0-10 */
    float difficulty;       /* 难度 0-10 */
    float time_required;    /* 所需时间(小时) */
    priority_quadrant_t quadrant;
    uint32_t sequence;      /* 建议学习顺序 */
} learning_priority_t;

/* 优先级计算 */
learning_priority_t *calculate_priorities(skill_item_t *skills,
                                          uint32_t count,
                                          uint32_t *result_count);

/* 获取学习建议顺序 */
learning_priority_t **get_recommended_sequence(learning_priority_t *priorities,
                                                uint32_t count,
                                                uint32_t *sequence_count);

/* 象限名称 */
static inline const char *quadrant_name(priority_quadrant_t q) {
    const char *names[] = {
        "🚀 快速收益 (Quick Wins)",
        "⭐ 重大项目 (Major Projects)",
        "📋 填充任务 (Fill-ins)",
        "⚠️  避免 (Thankless Tasks)"
    };
    return names[q];
}

#endif /* PRIORITY_MATRIX_H */
