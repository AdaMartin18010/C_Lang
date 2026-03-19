/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\03_Denotational_Semantics.md
 * Line: 341
 * Language: c
 * Block ID: b365be66
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 接上 */

/*
 * 语句语义: ⟦S⟧ : State → State
 * - 赋值: ⟦x := E⟧(σ) = σ[x ↦ ⟦E⟧(σ)]
 * - 顺序: ⟦S1; S2⟧(σ) = ⟦S2⟧(⟦S1⟧(σ))
 * - 条件: ⟦if B then S1 else S2⟧(σ) = ⟦S1⟧(σ) if ⟦B⟧(σ) else ⟦S2⟧(σ)
 */

// 赋值语句语义
State* semantic_assign(State *sigma, const char *var, int value) {
    State *new_sigma = state_copy(sigma);
    state_update(new_sigma, var, value);
    return new_sigma;
}

// 顺序语句语义
State* semantic_seq(State* (*s1)(State*), State* (*s2)(State*), State *sigma) {
    State *sigma1 = s1(sigma);
    State *sigma2 = s2(sigma1);
    free(sigma1);
    return sigma2;
}

// 示例: x := 5; y := x + 3
State* example_program(State *sigma) {
    State *s1 = semantic_assign(sigma, "x", 5);
    State *s2 = semantic_assign(s1, "y", state_lookup(s1, "x") + 3);
    free(s1);
    return s2;
}

int main(void) {
    printf("=== 状态语义 ===\n");

    State *sigma0 = new_state();
    printf("初始状态: {}\n");

    State *sigma_final = example_program(sigma0);
    printf("执行后: x=%d, y=%d\n",
           state_lookup(sigma_final, "x"),
           state_lookup(sigma_final, "y"));

    free(sigma0);
    free(sigma_final);
    return 0;
}
