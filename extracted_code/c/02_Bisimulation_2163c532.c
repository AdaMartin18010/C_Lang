/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 651
 * Language: c
 * Block ID: 2163c532
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：未归一化的概率比较
bool prob_equal_wrong(double p1, double p2) {
    return p1 == p2;  // 浮点直接相等比较！
}

// ✅ 正确：使用epsilon比较
bool prob_equal_correct(double p1, double p2, double epsilon) {
    return fabs(p1 - p2) < epsilon;
}

// 概率分布归一化检查
bool is_valid_distribution(double *dist, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        if (dist[i] < 0) return false;
        sum += dist[i];
    }
    return fabs(sum - 1.0) < 1e-9;
}
