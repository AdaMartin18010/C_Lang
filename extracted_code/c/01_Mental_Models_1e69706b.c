/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 437
 * Language: c
 * Block ID: 1e69706b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 代码可读性评估工具

typedef struct {
    int line_count;
    int comment_ratio;      // 注释行数/总行数
    int avg_identifier_len; // 平均标识符长度
    int nesting_depth;      // 最大嵌套深度
    int function_count;     // 函数数量
    int cyclomatic;         // 圈复杂度
} ReadabilityMetrics;

// 计算可读性分数
double calculate_readability(ReadabilityMetrics *m) {
    double score = 100.0;

    // 行数惩罚
    if (m->line_count > 500) score -= 10;
    if (m->line_count > 1000) score -= 20;

    // 注释比例（理想：15-30%）
    if (m->comment_ratio < 10) score -= 15;
    if (m->comment_ratio > 40) score -= 5;

    // 嵌套深度惩罚
    if (m->nesting_depth > 4) score -= (m->nesting_depth - 4) * 5;

    // 圈复杂度惩罚
    if (m->cyclomatic > 10) score -= (m->cyclomatic - 10) * 2;

    return score < 0 ? 0 : score;
}
