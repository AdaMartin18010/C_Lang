/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 536
 * Language: c
 * Block ID: 44b2781e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 水下延迟容忍网络（DTN）路由
typedef struct {
    uint32_t dest_id;
    uint32_t next_hop;
    float encounter_prob;    // 相遇概率
    uint32_t last_encounter;
    float delay_estimate;
} RoutingEntry;

// 基于相遇概率的路由（PROPHET 协议简化版）
void update_encounter_prob(RoutingEntry *entry, uint32_t current_time) {
    // 老化因子
    float gamma = 0.98f;
    uint32_t time_delta = current_time - entry->last_encounter;

    // 概率衰减
    entry->encounter_prob *= powf(gamma, time_delta);

    // 相遇更新
    entry->encounter_prob += (1 - entry->encounter_prob) * 0.5f;
    entry->last_encounter = current_time;
}

// 数据包转发决策
int should_forward(RoutingEntry *local, RoutingEntry *relay,
                   uint32_t dest_id) {
    // 如果中继节点到目标的相遇概率更高，则转发
    if (relay->encounter_prob > local->encounter_prob) {
        return 1;
    }
    return 0;
}
