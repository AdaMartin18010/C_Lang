/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 584
 * Language: c
 * Block ID: a7f56ae7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 边缘算子必须满足 ∂∘∂ = 0
bool verify_boundary_operator(BoundaryOperator *d) {
    // 检查所有2-链
    for (int i = 0; i < n_2chains; i++) {
        OneChain *b2 = boundary_2(&two_chains[i]);
        ZeroChain *b1b2 = boundary_1(b2);

        // ∂₁(∂₂(c)) 必须为零
        if (!is_zero(b1b2, 6)) {  // 3条边，每条有source/target
            fprintf(stderr, "边界算子不满足链复形条件!\n");
            return false;
        }
    }
    return true;
}
