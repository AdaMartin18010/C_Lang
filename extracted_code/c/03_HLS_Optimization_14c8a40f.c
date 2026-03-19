/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 651
 * Language: c
 * Block ID: 14c8a40f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 在满足性能要求下使用最低频率
 */

// 性能余量分析
void power_dvfs_analysis() {
    // 设计目标：1080p@30fps = 62.2M像素/秒
    // 实现能力：II=1 @ 100MHz = 100M像素/秒

    // 可以降低时钟频率到65MHz
    // 功耗降低约 (65/100)^2 ≈ 42%
}

// 动态性能调整
void power_adaptive(int in[1000], int out[1000], int quality_mode) {
    #pragma HLS INTERFACE mode=s_axilite port=quality_mode

    if (quality_mode == HIGH_QUALITY) {
        // 完全展开，高功耗
        for (int i = 0; i < 1000; i++) {
            #pragma HLS UNROLL factor=8
            out[i] = precise_algorithm(in[i]);
        }
    } else {
        // 顺序执行，低功耗
        for (int i = 0; i < 1000; i++) {
            #pragma HLS PIPELINE off
            out[i] = approximate_algorithm(in[i]);
        }
    }
}
