/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 548
 * Language: c
 * Block ID: bd5a105c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 建立时间和保持时间

typedef struct {
    uint32_t t_setup;    // 建立时间（时钟沿前数据必须稳定）
    uint32_t t_hold;     // 保持时间（时钟沿后数据必须稳定）
    uint32_t t_cq;       // 时钟到Q延迟
} TimingConstraint;

// 时序分析
Bool check_setup_time(uint32_t data_arrival, uint32_t clock_edge,
                       uint32_t t_setup) {
    // 数据必须在时钟沿前t_setup时间到达
    return (clock_edge - data_arrival) >= t_setup;
}

Bool check_hold_time(uint32_t data_change, uint32_t clock_edge,
                      uint32_t t_hold) {
    // 数据必须在时钟沿后保持t_hold时间不变
    return (data_change - clock_edge) >= t_hold;
}
