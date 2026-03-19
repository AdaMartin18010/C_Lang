/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 563
 * Language: c
 * Block ID: e00cf58e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：只考虑自由部分
void wrong_homology_analysis(ChainComplex *C) {
    HomologyGroup *H = compute_homology(1, C);
    // 只报告β₁，忽略挠子群
    printf("H₁ = Z^%d\n", H->rank);  // 可能不完整！
}

// 正确：完整报告同调群结构
void correct_homology_analysis(ChainComplex *C) {
    HomologyGroup *H = compute_homology(1, C);
    printf("H₁ = Z^%d", H->rank);
    for (int i = 0; i < H->n_torsion; i++) {
        printf(" ⊕ Z/%dZ", H->torsion[i]);
    }
    printf("\n");
}
