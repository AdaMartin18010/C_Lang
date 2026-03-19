/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 617
 * Language: c
 * Block ID: dbaf2c20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：在需要弱互模拟时使用强互模拟
bool check_equivalence_wrong(LTS *lts, int p, int q) {
    return strong_bisimilar(lts, p, q);  // 忽略了tau动作！
}

// ✅ 正确：根据需求选择
bool check_equivalence_correct(LTS *lts, int p, int q, bool consider_tau) {
    if (consider_tau) {
        return weak_bisimilar(lts, p, q);
    } else {
        return strong_bisimilar(lts, p, q);
    }
}
