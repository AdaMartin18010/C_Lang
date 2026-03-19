/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md
 * Line: 988
 * Language: c
 * Block ID: b923e88a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例：FIR滤波器的Pareto前沿

// 配置A：最小面积（顺序执行）
void fir_min_area(int x[1000], int y[1000], int coeff[16]) {
    // 1个乘法器，顺序执行
    // 延迟：16000周期
    // LUT: ~200, DSP: 1
}

// 配置B：平衡（部分展开）
void fir_balanced(int x[1000], int y[1000], int coeff[16]) {
    #pragma HLS PIPELINE II=4
    #pragma HLS UNROLL factor=4
    // 4个乘法器，II=4
    // 延迟：4000周期
    // LUT: ~600, DSP: 4
}

// 配置C：最大性能（完全展开）
void fir_max_perf(int x[1000], int y[1000], int coeff[16]) {
    #pragma HLS PIPELINE II=1
    #pragma HLS UNROLL
    // 16个乘法器，II=1
    // 延迟：1000周期
    // LUT: ~2000, DSP: 16
}

/*
Pareto前沿曲线：

延迟(周期)
   │
16K├────● 配置A（最小面积）
   │
 4K├──────────● 配置B（平衡）
   │
 1K├────────────────────● 配置C（最大性能）
   │
   └────┬─────┬────────┬──────────────▶ 资源(DSP数量)
        1     4        16

设计选择应根据应用需求在Pareto前沿上选择。
*/
