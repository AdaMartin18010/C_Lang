/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\02_Rule_Engine.md
 * Line: 135
 * Language: c
 * Block ID: 4c35e734
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 决策表: 条件组合 -> 动作 */

#define MAX_CONDITIONS 8
#define MAX_ACTIONS 4

typedef enum { COND_FALSE, COND_TRUE, COND_DONT_CARE } cond_value_t;

typedef struct {
    cond_value_t conditions[MAX_CONDITIONS];
    int action_id;
} rule_row_t;

typedef struct {
    int condition_count;
    int action_count;
    rule_row_t* rows;
    int row_count;
} decision_table_t;

/* 评估决策表 */
int decision_table_evaluate(decision_table_t* table, bool* facts) {
    for (int i = 0; i < table->row_count; i++) {
        rule_row_t* row = &table->rows[i];
        bool match = true;

        for (int j = 0; j < table->condition_count; j++) {
            if (row->conditions[j] == COND_DONT_CARE) continue;

            bool expected = (row->conditions[j] == COND_TRUE);
            if (facts[j] != expected) {
                match = false;
                break;
            }
        }

        if (match) {
            return row->action_id;
        }
    }
    return -1;  /* 无匹配 */
}

/* 示例: 保险费率决策 */
/* 条件: [年龄>25, 有驾照, 无事故记录, 驾龄>3年] */
/* 费率: 1=高, 2=中, 3=低 */
decision_table_t insurance_table = {
    .condition_count = 4,
    .action_count = 3,
    .rows = (rule_row_t[]){
        {{COND_FALSE, COND_TRUE,  COND_DONT_CARE, COND_DONT_CARE}, 1},  /* 年轻: 高 */
        {{COND_TRUE,  COND_TRUE,  COND_FALSE,      COND_DONT_CARE}, 2},  /* 有事故: 中 */
        {{COND_TRUE,  COND_TRUE,  COND_TRUE,       COND_FALSE},      2},  /* 驾龄短: 中 */
        {{COND_TRUE,  COND_TRUE,  COND_TRUE,       COND_TRUE},       3},  /* 全部满足: 低 */
    },
    .row_count = 4
};
