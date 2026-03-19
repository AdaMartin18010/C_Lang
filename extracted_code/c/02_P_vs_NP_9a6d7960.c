/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_P_vs_NP.md
 * Line: 373
 * Language: c
 * Block ID: 9a6d7960
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DPLL算法 - 最坏情况指数时间
bool dpll(Formula* f) {
    // 单位传播
    if (!unit_propagation(f)) return false;

    // 纯文字消除
    pure_literal_elimination(f);

    // 检查是否满足
    if (is_empty(f)) return true;
    if (has_empty_clause(f)) return false;

    // 选择分支变量
    int var = choose_variable(f);

    // 分支：尝试赋值为真/假
    Formula* f_true = copy_and_assign(f, var, true);
    Formula* f_false = copy_and_assign(f, var, false);

    return dpll(f_true) || dpll(f_false);
}
