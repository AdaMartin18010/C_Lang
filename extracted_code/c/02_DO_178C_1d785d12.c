/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\02_DO_178C.md
 * Line: 467
 * Language: c
 * Block ID: 1d785d12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file coverage_analyzer.c
 * @brief DO-178C结构化覆盖率分析实现
 * @software_level A
 * @coverage_required MC/DC
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* 最大支持的条件数 */
#define MAX_CONDITIONS              (32U)
#define MAX_DECISIONS               (256U)
#define MAX_TEST_CASES              (1024U)

/* 条件类型 */
typedef enum {
    COND_TYPE_BOOLEAN = 0,          /* 布尔条件 */
    COND_TYPE_RELATIONAL = 1,       /* 关系条件 */
    COND_TYPE_COMPOUND = 2          /* 复合条件 */
} ConditionType_t;

/* 条件结构体 */
typedef struct {
    char name[32];
    ConditionType_t type;
    bool values[2];                 /* 可能的值: false, true */
    uint32_t true_count;
    uint32_t false_count;
} Condition_t;

/* 判定结构体 */
typedef struct {
    char name[64];
    uint32_t condition_indices[MAX_CONDITIONS];
    uint32_t condition_count;
    char operator[4];               /* "&&", "||" */
    uint32_t true_outcome_count;
    uint32_t false_outcome_count;
    bool mcdc_achieved;
} Decision_t;

/* 测试用例 */
typedef struct {
    uint32_t id;
    bool condition_values[MAX_CONDITIONS];
    uint32_t condition_count;
    bool decision_result;
    bool used_for_mcdc[MAX_CONDITIONS];
} TestCase_t;

/* 覆盖率数据库 */
typedef struct {
    Decision_t decisions[MAX_DECISIONS];
    uint32_t decision_count;
    Condition_t conditions[MAX_CONDITIONS];
    uint32_t condition_count;
    TestCase_t test_cases[MAX_TEST_CASES];
    uint32_t test_case_count;
} CoverageDatabase_t;

static CoverageDatabase_t g_cov_db;

/**
 * @brief 初始化覆盖率数据库
 */
bool Coverage_Init(void) {
    (void)memset(&g_cov_db, 0, sizeof(g_cov_db));
    return true;
}

/**
 * @brief 注册判定
 * @param name 判定名称
 * @param operator 逻辑运算符
 * @return 判定索引
 */
int32_t Coverage_RegisterDecision(const char *name, const char *operator) {
    if (g_cov_db.decision_count >= MAX_DECISIONS) {
        return -1;
    }

    (void)strncpy(g_cov_db.decisions[g_cov_db.decision_count].name,
                   name, 63U);
    g_cov_db.decisions[g_cov_db.decision_count].name[63U] = '\0';

    (void)strncpy(g_cov_db.decisions[g_cov_db.decision_count].operator,
                   operator, 3U);
    g_cov_db.decisions[g_cov_db.decision_count].operator[3U] = '\0';

    g_cov_db.decision_count++;
    return (int32_t)(g_cov_db.decision_count - 1U);
}

/**
 * @brief 向判定添加条件
 */
bool Coverage_AddConditionToDecision(uint32_t decision_idx,
                                      const char *cond_name,
                                      ConditionType_t type) {
    uint32_t cond_idx;
    Decision_t *dec;

    if (decision_idx >= g_cov_db.decision_count) {
        return false;
    }

    /* 注册新条件 */
    if (g_cov_db.condition_count >= MAX_CONDITIONS) {
        return false;
    }

    cond_idx = g_cov_db.condition_count;
    (void)strncpy(g_cov_db.conditions[cond_idx].name, cond_name, 31U);
    g_cov_db.conditions[cond_idx].name[31U] = '\0';
    g_cov_db.conditions[cond_idx].type = type;
    g_cov_db.conditions[cond_idx].values[0] = false;
    g_cov_db.conditions[cond_idx].values[1] = true;
    g_cov_db.condition_count++;

    /* 关联到判定 */
    dec = &g_cov_db.decisions[decision_idx];
    if (dec->condition_count >= MAX_CONDITIONS) {
        return false;
    }

    dec->condition_indices[dec->condition_count] = cond_idx;
    dec->condition_count++;

    return true;
}

/**
 * @brief 记录测试用例执行
 */
bool Coverage_RecordTestCase(uint32_t decision_idx,
                              const bool *cond_values,
                              uint32_t num_conditions,
                              bool result) {
    TestCase_t *tc;
    uint32_t i;

    if ((decision_idx >= g_cov_db.decision_count) ||
        (g_cov_db.test_case_count >= MAX_TEST_CASES)) {
        return false;
    }

    tc = &g_cov_db.test_cases[g_cov_db.test_case_count];
    tc->id = g_cov_db.test_case_count;
    tc->decision_result = result;
    tc->condition_count = num_conditions;

    for (i = 0U; i < num_conditions && i < MAX_CONDITIONS; i++) {
        tc->condition_values[i] = cond_values[i];
    }

    g_cov_db.test_case_count++;

    /* 更新计数 */
    if (result) {
        g_cov_db.decisions[decision_idx].true_outcome_count++;
    } else {
        g_cov_db.decisions[decision_idx].false_outcome_count++;
    }

    return true;
}

/**
 * @brief MC/DC分析
 * @note 验证每个条件独立影响判定结果
 */
bool Coverage_AnalyzeMCDC(uint32_t decision_idx) {
    Decision_t *dec;
    bool independent[MAX_CONDITIONS] = {false};
    uint32_t i, j, k;
    uint32_t indep_count = 0U;

    if (decision_idx >= g_cov_db.decision_count) {
        return false;
    }

    dec = &g_cov_db.decisions[decision_idx];

    /* 对每个条件寻找独立影响判定结果的测试用例对 */
    for (i = 0U; i < dec->condition_count; i++) {
        for (j = 0U; j < g_cov_db.test_case_count; j++) {
            for (k = j + 1U; k < g_cov_db.test_case_count; k++) {
                TestCase_t *tc1 = &g_cov_db.test_cases[j];
                TestCase_t *tc2 = &g_cov_db.test_cases[k];
                bool differs_only_in_i = true;
                uint32_t m;

                /* 检查是否只有条件i不同 */
                for (m = 0U; m < dec->condition_count; m++) {
                    if (m == i) {
                        if (tc1->condition_values[m] == tc2->condition_values[m]) {
                            differs_only_in_i = false;
                            break;
                        }
                    } else {
                        if (tc1->condition_values[m] != tc2->condition_values[m]) {
                            differs_only_in_i = false;
                            break;
                        }
                    }
                }

                /* 检查判定结果是否不同 */
                if (differs_only_in_i &&
                    (tc1->decision_result != tc2->decision_result)) {
                    independent[i] = true;
                    tc1->used_for_mcdc[i] = true;
                    tc2->used_for_mcdc[i] = true;
                    break;
                }
            }
            if (independent[i]) {
                break;
            }
        }

        if (independent[i]) {
            indep_count++;
        }
    }

    /* 检查是否所有条件都独立 */
    dec->mcdc_achieved = (indep_count == dec->condition_count);

    return dec->mcdc_achieved;
}

/**
 * @brief 生成覆盖率报告
 */
void Coverage_GenerateReport(void) {
    uint32_t i;

    /* 判定覆盖报告 */
    for (i = 0U; i < g_cov_db.decision_count; i++) {
        Decision_t *dec = &g_cov_db.decisions[i];
        bool decision_covered = (dec->true_outcome_count > 0U) &&
                                (dec->false_outcome_count > 0U);

        /* 这里可以输出到日志或文件 */
        (void)decision_covered;  /* 防止未使用警告 */
    }
}
