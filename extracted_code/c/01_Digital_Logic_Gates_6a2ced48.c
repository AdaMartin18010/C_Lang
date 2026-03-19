/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 268
 * Language: c
 * Block ID: 6a2ced48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 门延迟模型（单位：皮秒 ps）
typedef struct {
    uint32_t tplh;  // 低到高传输延迟
    uint32_t tphl;  // 高到低传输延迟
    uint32_t tr;    // 上升时间
    uint32_t tf;    // 下降时间
} GateDelay;

// 典型延迟值（45nm工艺，近似值）
GateDelay delay_nand2 = {  // 2输入NAND
    .tplh = 25,  // ps
    .tphl = 20,
    .tr = 15,
    .tf = 12
};

GateDelay delay_inv = {   // 反相器
    .tplh = 15,
    .tphl = 12,
    .tr = 10,
    .tf = 8
};

// 传播延迟计算
uint32_t calculate_path_delay(GateDelay *gates, int count) {
    uint32_t total = 0;
    for (int i = 0; i < count; i++) {
        // 取平均传播延迟
        total += (gates[i].tplh + gates[i].tphl) / 2;
    }
    return total;
}
