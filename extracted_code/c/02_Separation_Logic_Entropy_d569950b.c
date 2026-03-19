/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 279
 * Language: c
 * Block ID: d569950b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 帧规则（Frame Rule）是分离逻辑的核心：
 *
 *       {P} C {Q}
 *   -------------------  (mod(C) ∩ fv(R) = ∅)
 *   {P * R} C {Q * R}
 *
 * 含义：如果C在P下从Q正确执行，
 * 那么C在P*R下执行，R部分保持不变，结果满足Q*R
 *
 * 关键约束：C不修改R中的变量
 */

// 帧规则应用示例
/*
原始规范：
  {x ↦ 3} [x] := 5 {x ↦ 5}

应用帧规则（R = y ↦ 7）：
  {x ↦ 3 * y ↦ 7} [x] := 5 {x ↦ 5 * y ↦ 7}

含义：修改x不影响y
*/

// 形式化帧规则
typedef struct {
    SL_Assertion precondition;
    Stmt *command;
    SL_Assertion postcondition;
    GHashTable *modified_vars;
} HoareTriple;

bool check_frame_rule(HoareTriple *base, SL_Assertion R) {
    // 检查R的free变量与C的修改变量不相交
    GHashTable *R_fv = free_variables(R);

    GHashTableIter iter;
    gpointer var;
    g_hash_table_iter_init(&iter, R_fv);
    while (g_hash_table_iter_next(&iter, &var, NULL)) {
        if (g_hash_table_contains(base->modified_vars, var)) {
            return FALSE;
        }
    }

    return TRUE;
}

HoareTriple *apply_frame_rule(HoareTriple *base, SL_Assertion R) {
    HoareTriple *framed = malloc(sizeof(HoareTriple));
    framed->precondition = sl_sep_conj(base->precondition, R);
    framed->command = base->command;
    framed->postcondition = sl_sep_conj(base->postcondition, R);
    framed->modified_vars = base->modified_vars;
    return framed;
}
