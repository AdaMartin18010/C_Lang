/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\05_Computational_Complexity\01_Complexity_Classes.md
 * Line: 119
 * Language: c
 * Block ID: 3c5ecc17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 布尔可满足性问题
// 给定: 布尔公式 (x1 OR NOT x2) AND (x2 OR x3)
// 问: 是否存在使公式为真的赋值?

// 验证解容易 (多项式时间)
bool verify_sat(const Formula* f, const Assignment* a) {
    return evaluate(f, a);  // O(公式长度)
}

// 但找解可能很难 (目前无已知多项式算法)
bool solve_sat(const Formula* f, Assignment* out) {
    // 最坏情况需要尝试2^n种赋值
    return brute_force_sat(f, out);  // 指数时间
}
