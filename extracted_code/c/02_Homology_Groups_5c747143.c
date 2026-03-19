/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 310
 * Language: c
 * Block ID: 5c747143
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算各阶Betti数
void compute_betti_numbers(const ChainComplex *complex, int *betti) {
    // βₙ = dim(ker ∂ₙ) - dim(im ∂ₙ₊₁)
    //    = nullity(∂ₙ) - rank(∂ₙ₊₁)

    for (int n = 0; n < complex->max_dimension; n++) {
        Matrix boundary_n = get_boundary_matrix(complex, n);
        Matrix boundary_nplus1 = get_boundary_matrix(complex, n + 1);

        int nullity_n = compute_nullity(boundary_n);
        int rank_nplus1 = compute_rank(boundary_nplus1);

        betti[n] = nullity_n - rank_nplus1;
    }
}

// Euler示性数: χ = Σ(-1)ⁿ βₙ
int compute_euler_characteristic(const int *betti, int max_dim) {
    int chi = 0;
    for (int n = 0; n <= max_dim; n++) {
        chi += (n % 2 == 0 ? 1 : -1) * betti[n];
    }
    return chi;
}

// 软件架构的拓扑度量
typedef struct {
    int n_modules;          // 模块数
    int n_references;       // 引用数
    int beta_0;             // 连通分量数
    int beta_1;             // 循环依赖数
    int beta_2;             // 2维复杂度
    float cyclomatic_topo;  // 拓扑圈复杂度
} ArchitectureTopology;

ArchitectureTopology analyze_architecture(const char *project_path) {
    ArchitectureTopology topo = {0};

    // 解析代码依赖图
    DependencyGraph graph = parse_dependencies(project_path);

    // 构建链复形
    ChainComplex complex = dependency_to_chain_complex(graph);

    // 计算Betti数
    int betti[3];
    compute_betti_numbers(&complex, betti);
    topo.beta_0 = betti[0];
    topo.beta_1 = betti[1];
    topo.beta_2 = betti[2];

    // 计算拓扑圈复杂度
    // 类似图论中的 M = E - N + 2P
    topo.cyclomatic_topo = graph.n_edges - graph.n_nodes + 2 * betti[0];

    return topo;
}
