/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 200
 * Language: c
 * Block ID: 2cdbcb33
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 同调群计算

typedef struct {
    int dimension;
    int rank;           // 自由部分的秩（Betti数）
    int *torsion;       // 挠子群
    int n_torsion;
} HomologyGroup;

// 计算核 ker(∂ₙ)
ChainGroup *compute_kernel(
    const ChainGroup *domain,
    ChainGroup *(*boundary)(const void *)
) {
    // 求解 ∂ₙ(c) = 0
    // 转化为线性代数问题
    Matrix mat = boundary_to_matrix(boundary, domain);
    Matrix null_space = compute_null_space(mat);

    return matrix_to_chain_group(null_space);
}

// 计算像 im(∂ₙ₊₁)
ChainGroup *compute_image(
    const ChainGroup *codomain,
    ChainGroup *(*boundary)(const void *)
) {
    // im(∂ₙ₊₁) = { ∂ₙ₊₁(c) | c ∈ Cₙ₊₁ }
    Matrix mat = boundary_to_matrix(boundary, codomain);
    Matrix col_space = compute_column_space(mat);

    return matrix_to_chain_group(col_space);
}

// 商群计算: Hₙ = ker(∂ₙ) / im(∂ₙ₊₁)
HomologyGroup *compute_homology(
    int n,
    const ChainGroup *C_n,      // n-链群
    const ChainGroup *C_nplus1  // (n+1)-链群
) {
    ChainGroup *ker = compute_kernel(C_n, get_boundary_op(n));
    ChainGroup *im = compute_image(C_nplus1, get_boundary_op(n + 1));

    // 计算商群
    // Hₙ ≅ ℤ^β ⊕ (挠子群)
    HomologyGroup *H = malloc(sizeof(HomologyGroup));
    H->dimension = n;
    H->rank = ker->n_generators - im->n_generators;  // βₙ

    // 计算挠子群（Smith标准形）
    Matrix relation = compute_relation_matrix(ker, im);
    SmithNormalForm snf = compute_smith_normal_form(relation);
    H->torsion = extract_torsion(snf);
    H->n_torsion = snf.n_nonunit;

    return H;
}
