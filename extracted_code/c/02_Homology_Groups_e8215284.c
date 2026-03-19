/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 132
 * Language: c
 * Block ID: e8215284
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 边缘算子 ∂: Cₙ → Cₙ₋₁

// ∂₀: C₀ → 0（平凡映射）
void boundary_0(const ZeroChain *c, void *result) {
    // 总是映射到0
    *(int *)result = 0;
}

// ∂₁: C₁ → C₀
// 对于引用 r: source → target
// ∂₁(r) = target - source（形式差）
ZeroChain *boundary_1(const OneChain *c) {
    // 代数上: ∂(source→target) = target - source
    ZeroChain *result = malloc(sizeof(ZeroChain) * 2);
    result[0] = *c->target;   // +target
    result[1] = *c->source;   // -source
    result[1].address = -result[1].address;  // 取负
    return result;
}

// ∂₂: C₂ → C₁
// 对于三角形 (e₁, e₂, e₃)
// ∂₂(e₁, e₂, e₃) = e₁ + e₂ + e₃（有向边之和）
OneChain *boundary_2(const TwoChain *c) {
    // 验证: ∂₁∘∂₂ = 0
    // ∂₁(∂₂(triangle)) = ∂₁(e₁+e₂+e₃)
    //                  = (B-A) + (C-B) + (A-C) = 0 ✓
    OneChain *result = malloc(sizeof(OneChain) * 3);
    for (int i = 0; i < 3; i++) {
        result[i] = *c->edges[i];
    }
    return result;
}

// 验证链复形性质: ∂∘∂ = 0
bool verify_chain_complex_property(void) {
    // 测试: ∂₁(∂₂(triangle)) = 0
    TwoChain test_triangle = create_test_triangle();
    OneChain *boundary2_result = boundary_2(&test_triangle);

    // 应用∂₁到每条边并求和
    int64_t sum = 0;
    for (int i = 0; i < 3; i++) {
        ZeroChain *bd = boundary_1(&boundary2_result[i]);
        sum += bd[0].address + bd[1].address;  // target - source
        free(bd);
    }

    free(boundary2_result);
    return sum == 0;  // 应该为0
}
