/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 376
 * Language: c
 * Block ID: cb395ea5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Filtration: ∅ = K₀ ⊆ K₁ ⊆ ... ⊆ Kₙ = K
typedef struct {
    double scale;           // 时间/复杂度阈值
    ChainComplex complex;   // 当前尺度的链复形
} FiltrationStep;

typedef struct {
    FiltrationStep *steps;
    int n_steps;
} Filtration;

// 持续性图 (barcode)
typedef struct {
    int dimension;
    double birth;       // 出现时间
    double death;       // 消失时间（∞表示持久）
} Barcode;

// 计算持久同调
Barcode *compute_persistent_homology(const Filtration *filt, int *n_bars) {
    // 使用矩阵约化算法
    Matrix *boundary_mats = malloc(filt->n_steps * sizeof(Matrix));

    for (int i = 0; i < filt->n_steps; i++) {
        boundary_mats[i] = build_boundary_matrix(&filt->steps[i].complex);
    }

    // 持久对算法
    int *lowest_one = calloc(filt->n_steps, sizeof(int));
    Barcode *barcodes = malloc(filt->n_steps * sizeof(Barcode));
    *n_bars = 0;

    for (int j = 0; j < filt->n_steps; j++) {
        // 矩阵约化...
        while (lowest_one[j] != -1) {
            int i = lowest_one[j];
            if (lowest_one[i] == -1) {
                lowest_one[i] = j;
                break;
            } else {
                add_column(boundary_mats[i], boundary_mats[j]);
                lowest_one[j] = get_lowest_one(boundary_mats[j]);
            }
        }

        if (lowest_one[j] == -1) {
            // 新同调类的诞生
            barcodes[*n_bars] = (Barcode){
                .dimension = get_dimension(j),
                .birth = filt->steps[j].scale,
                .death = INFINITY
            };
            (*n_bars)++;
        } else {
            // 同调类的死亡
            int i = lowest_one[j];
            // 找到birth为i的barcode，设置death
            for (int k = 0; k < *n_bars; k++) {
                if (barcodes[k].birth == filt->steps[i].scale) {
                    barcodes[k].death = filt->steps[j].scale;
                    break;
                }
            }
        }
    }

    return barcodes;
}
