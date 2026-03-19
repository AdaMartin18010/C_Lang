/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\README.md
 * Line: 74
 * Language: c
 * Block ID: cbd1d265
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* skill_matrix.h - 技能矩阵核心定义 */
#ifndef SKILL_MATRIX_H
#define SKILL_MATRIX_H

#include <stdint.h>
#include <stdbool.h>

/* 技能等级 */
typedef enum {
    SKILL_LEVEL_NONE = 0,       /* 未接触 */
    SKILL_LEVEL_AWARENESS,      /* 了解概念 */
    SKILL_LEVEL_BASIC,          /* 基础应用 */
    SKILL_LEVEL_INTERMEDIATE,   /* 熟练应用 */
    SKILL_LEVEL_ADVANCED,       /* 深入理解 */
    SKILL_LEVEL_EXPERT,         /* 精通/能教学 */
    SKILL_LEVEL_MASTER          /* 领域权威 */
} skill_level_t;

/* 技能维度 */
typedef enum {
    SKILL_DIM_SYNTAX = 0,       /* 语法维度 */
    SKILL_DIM_STANDARD,         /* 标准库 */
    SKILL_DIM_SYSTEM,           /* 系统编程 */
    SKILL_DIM_TOOLS,            /* 工具链 */
    SKILL_DIM_DEBUGGING,        /* 调试能力 */
    SKILL_DIM_OPTIMIZATION,     /* 性能优化 */
    SKILL_DIM_ARCHITECTURE,     /* 架构设计 */
    SKILL_DIM_TESTING,          /* 测试能力 */
    SKILL_DIM_SECURITY,         /* 安全编程 */
    SKILL_DIM_COUNT
} skill_dimension_t;

/* 技能项 */
typedef struct {
    uint32_t id;
    const char *name;
    const char *description;
    skill_dimension_t dimension;
    skill_level_t current_level;
    skill_level_t target_level;
    uint32_t priority;          /* 1-10, 数值越高越优先 */
    uint32_t learning_hours;    /* 预计学习时间 */
    bool is_prerequisite;       /* 是否前置技能 */
    uint32_t *prereq_ids;       /* 前置技能ID列表 */
    uint32_t prereq_count;
} skill_item_t;

/* 技能矩阵 */
typedef struct {
    const char *name;
    skill_item_t *items;
    uint32_t item_count;
    uint32_t item_capacity;

    /* 矩阵统计 */
    uint32_t level_distribution[8];  /* 各等级技能数量 */
    float overall_score;             /* 综合评分 0-100 */
    float coverage_rate;             /* 覆盖率 */
} skill_matrix_t;

/* API */
skill_matrix_t *sm_create(const char *name);
void sm_destroy(skill_matrix_t *matrix);

int sm_add_skill(skill_matrix_t *matrix, skill_item_t *skill);
skill_item_t *sm_find_skill(skill_matrix_t *matrix, uint32_t id);
int sm_update_level(skill_matrix_t *matrix, uint32_t id, skill_level_t level);

/* 分析 */
float sm_calculate_overall_score(skill_matrix_t *matrix);
float sm_calculate_coverage(skill_matrix_t *matrix);
skill_item_t **sm_find_gaps(skill_matrix_t *matrix, uint32_t *count);
skill_item_t **sm_get_learning_path(skill_matrix_t *matrix, uint32_t *count);

/* 报告生成 */
int sm_generate_report(skill_matrix_t *matrix, char *buffer, size_t buffer_size);

/* 技能等级字符串 */
static inline const char *skill_level_string(skill_level_t level) {
    const char *names[] = {
        "未接触", "了解", "基础", "中级", "高级", "专家", "大师"
    };
    return (level < 7) ? names[level] : "未知";
}

#endif /* SKILL_MATRIX_H */
