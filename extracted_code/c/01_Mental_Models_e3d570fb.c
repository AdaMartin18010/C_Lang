/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 302
 * Language: c
 * Block ID: e3d570fb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 圈复杂度 = 决策点数量 + 1
// 高圈复杂度 = 高认知负荷

// 圈复杂度 = 5（可管理）
int calculate_grade_bad(int score) {
    if (score >= 90) return 'A';
    if (score >= 80) return 'B';
    if (score >= 70) return 'C';
    if (score >= 60) return 'D';
    return 'F';
}

// 使用查找表降低认知负荷
// 圈复杂度 = 1
int calculate_grade_good(int score) {
    static const int thresholds[] = {90, 80, 70, 60, 0};
    static const char grades[] = {'A', 'B', 'C', 'D', 'F'};

    for (int i = 0; i < 5; i++) {
        if (score >= thresholds[i]) return grades[i];
    }
    return 'F';
}
