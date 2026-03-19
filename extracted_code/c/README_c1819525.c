/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 433
 * Language: c
 * Block ID: c1819525
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传感器节点数据结构
typedef struct {
    uint32_t node_id;
    float temperature;
    float pressure;
    float salinity;
    float dissolved_oxygen;
    uint64_t timestamp;
    float battery_level;
} SensorNodeData;

// 时间同步（使用水声传播延迟补偿）
typedef struct {
    float x, y, z;           // 节点位置
    float clock_offset;      // 时钟偏移
    float clock_drift;       // 时钟漂移
} NodeClockSync;

// 单向时间同步
void oneway_time_sync(NodeClockSync *node,
                      float sender_time,
                      float arrival_time) {
    // 估算传播延迟（需要预先知道距离）
    float distance = sqrtf(node->x*node->x + node->y*node->y +
                           node->z*node->z);
    float propagation_delay = distance / SOUND_SPEED_WATER;

    // 计算时钟偏移
    node->clock_offset = sender_time + propagation_delay - arrival_time;
}

// 双向时间同步（更精确）
float twoway_time_sync(NodeClockSync *node_a, NodeClockSync *node_b,
                       float t1, float t2, float t3, float t4) {
    // t1: A 发送时间, t2: B 接收时间
    // t3: B 发送时间, t4: A 接收时间

    float propagation_delay = ((t2 - t1) + (t4 - t3)) / 2.0f;
    float clock_offset = ((t2 - t1) - (t4 - t3)) / 2.0f;

    return clock_offset;
}
