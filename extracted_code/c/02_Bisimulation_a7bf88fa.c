/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 635
 * Language: c
 * Block ID: a7bf88fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：只检查单向模拟
bool is_simulation_wrong(LTS *lts, Relation *R) {
    // 只检查p模拟q
    return check_simulation_one_way(lts, R, p_to_q);
}

// ✅ 正确：互模拟需要双向检查
bool is_bisimulation_correct(LTS *lts, Relation *R) {
    return check_simulation_one_way(lts, R, p_to_q) &&
           check_simulation_one_way(lts, R, q_to_p);
}
