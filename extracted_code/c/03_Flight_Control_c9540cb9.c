/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\03_Flight_Control.md
 * Line: 103
 * Language: c
 * Block ID: c9540cb9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 三余度表决
typedef struct {
    float value[3];  // 三个通道的值
    uint8_t status[3]; // 通道状态
} triplex_data_t;

float vote(triplex_data_t* data) {
    // 中值表决
    float a = data->value[0];
    float b = data->value[1];
    float c = data->value[2];

    // 返回中值
    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

// 故障检测
uint8_t detect_fault(triplex_data_t* data, float threshold) {
    float diff01 = fabsf(data->value[0] - data->value[1]);
    float diff12 = fabsf(data->value[1] - data->value[2]);
    float diff02 = fabsf(data->value[0] - data->value[2]);

    uint8_t fault_mask = 0;
    if (diff01 > threshold && diff02 > threshold) fault_mask |= 0x01;
    if (diff01 > threshold && diff12 > threshold) fault_mask |= 0x02;
    if (diff02 > threshold && diff12 > threshold) fault_mask |= 0x04;

    return fault_mask;
}
