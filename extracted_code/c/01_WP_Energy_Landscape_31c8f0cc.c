/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\01_WP_Energy_Landscape.md
 * Line: 205
 * Language: c
 * Block ID: 31c8f0cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 验证条件生成器（后向）
// 输入：程序S和后条件Q
// 输出：验证条件集合

typedef struct {
    Predicate precondition;
    GArray *verification_conditions;  // 需要证明的引理
} VCG_Result;

// 后向VCG实现
VCG_Result *vcg_backward(Stmt *S, Predicate Q) {
    VCG_Result *result = malloc(sizeof(VCG_Result));
    result->verification_conditions = g_array_new(FALSE, FALSE, sizeof(Predicate));

    switch (S->type) {
        case STMT_SKIP:
            result->precondition = Q;
            break;

        case STMT_ASSIGN:
            result->precondition = substitute(Q, S->assign.var, S->assign.expr);
            break;

        case STMT_SEQ:
            // 先处理S2，再处理S1
            VCG_Result *r2 = vcg_backward(S->seq.s2, Q);
            VCG_Result *r1 = vcg_backward(S->seq.s1, r2->precondition);

            result->precondition = r1->precondition;
            g_array_append_vals(result->verification_conditions,
                               r2->verification_conditions->data,
                               r2->verification_conditions->len);
            g_array_append_vals(result->verification_conditions,
                               r1->verification_conditions->data,
                               r1->verification_conditions->len);
            break;

        case STMT_IF:
            VCG_Result *r_then = vcg_backward(S->if_stmt.then_branch, Q);
            VCG_Result *r_else = vcg_backward(S->if_stmt.else_branch, Q);

            // (B ∧ WP[then]) ∨ (¬B ∧ WP[else])
            // 生成条件：B ⇒ WP_then 和 ¬B ⇒ WP_else
            result->precondition = and(
                implies(S->if_stmt.condition, r_then->precondition),
                implies(not(S->if_stmt.condition), r_else->precondition)
            );
            break;

        case STMT_WHILE: {
            // 需要用户提供循环不变式
            Predicate I = S->while_stmt.invariant;

            // 验证条件1：不变式初始成立
            // 这是检查点，不是WP的一部分
            Predicate vc_init = implies(result->precondition, I);
            g_array_append_val(result->verification_conditions, vc_init);

            // 验证条件2：不变式保持
            VCG_Result *r_body = vcg_backward(S->while_stmt.body, I);
            Predicate vc_preserve = implies(
                and(I, S->while_stmt.condition),
                r_body->precondition
            );
            g_array_append_val(result->verification_conditions, vc_preserve);

            // 验证条件3：退出时满足后条件
            Predicate vc_exit = implies(
                and(I, not(S->while_stmt.condition)),
                Q
            );
            g_array_append_val(result->verification_conditions, vc_exit);

            // WP = I（进入循环前需要满足不变式）
            result->precondition = I;
            break;
        }
    }

    return result;
}
