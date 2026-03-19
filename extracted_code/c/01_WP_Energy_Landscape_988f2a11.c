/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\01_WP_Energy_Landscape.md
 * Line: 291
 * Language: c
 * Block ID: 988f2a11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 最强后条件（Strongest Postcondition, SP）
 * sp(S, P) = 从满足P的状态执行S后必然满足的条件
 *
 * SP和WP是Galois连接：
 *   sp(S, P) ⊆ Q  ⟺  P ⊆ wp(S, Q)
 */

// 前向VCG：从前提推导后置条件
Predicate sp_forward(Stmt *S, Predicate P) {
    switch (S->type) {
        case STMT_SKIP:
            return P;

        case STMT_ASSIGN:
            // sp(x := E, P) = ∃v. x = E[v/x] ∧ P[v/x]
            // 即：存在原值v使得x等于E（用v替换x后的值）且P对v成立
            return exists("v",
                and(
                    equals(S->assign.var,
                          substitute(S->assign.expr, S->assign.var, "v")),
                    substitute(P, S->assign.var, "v")
                ));

        case STMT_SEQ:
            // sp(S1; S2, P) = sp(S2, sp(S1, P))
            return sp_forward(S->seq.s2, sp_forward(S->seq.s1, P));

        case STMT_IF:
            // sp(if B then S1 else S2, P) =
            //   sp(S1, P ∧ B) ∨ sp(S2, P ∧ ¬B)
            return or(
                sp_forward(S->if_stmt.then_branch,
                          and(P, S->if_stmt.condition)),
                sp_forward(S->if_stmt.else_branch,
                          and(P, not(S->if_stmt.condition)))
            );

        case STMT_WHILE:
            // 前向SP对循环较难处理，通常使用不变式
            // sp(while B do S, P) = I ∧ ¬B
            // 其中I是满足P ⇒ I且{I ∧ B} S {I}的不变式
            return and(S->while_stmt.invariant,
                      not(S->while_stmt.condition));
    }
    return P;
}

// 前向验证示例
/*
程序：
  x := 0;
  y := 0;
  while x < n do
    x := x + 1;
    y := y + x

前提：n ≥ 0
目标：验证后条件 y = n*(n+1)/2

SP计算：
  sp(x := 0, n ≥ 0) = n ≥ 0 ∧ x = 0
  sp(y := 0, n ≥ 0 ∧ x = 0) = n ≥ 0 ∧ x = 0 ∧ y = 0

循环不变式：y = x*(x+1)/2 ∧ x ≤ n

退出时：y = x*(x+1)/2 ∧ x ≤ n ∧ x ≥ n
        = y = n*(n+1)/2 ∧ x = n

证毕。
*/
