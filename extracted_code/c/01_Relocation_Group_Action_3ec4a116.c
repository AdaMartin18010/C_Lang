/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 504
 * Language: c
 * Block ID: 3ec4a116
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 重定位方程组的求解
// 系统: R·S = V, 其中R是重定位矩阵，S是符号向量，V是目标值

typedef struct {
    int n_equations;
    int n_symbols;
    // 稀疏矩阵表示
    struct {
        int row;
        int col;
        int64_t coeff;  // 系数（通常是1, -1, 0）
    } *entries;
    int n_entries;
} RelocSystem;

// 高斯消元法求解
bool solve_relocation_system(
    RelocSystem *sys,
    int64_t *solution,
    int64_t *target_values
) {
    // 构造增广矩阵
    Matrix aug = create_augmented_matrix(sys, target_values);

    // 高斯消元
    for (int i = 0; i < sys->n_equations; i++) {
        // 寻找主元
        int pivot = find_pivot(aug, i);
        if (pivot < 0) continue;  // 奇异矩阵

        // 行交换
        swap_rows(aug, i, pivot);

        // 归一化
        normalize_row(aug, i);

        // 消去
        for (int j = 0; j < sys->n_equations; j++) {
            if (j != i) {
                eliminate(aug, j, i);
            }
        }
    }

    // 提取解
    for (int i = 0; i < sys->n_symbols; i++) {
        solution[i] = aug.data[i][sys->n_symbols];
    }

    return true;
}
