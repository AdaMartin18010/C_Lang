/*
 * Auto-generated from: 16_Embedded_AI_Agents\01_TinyML_Fundamentals\01_Introduction_to_TinyML.md
 * Line: 99
 * Language: c
 * Block ID: f22535b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例: 从IMU传感器收集数据
#include "sensor.h"

#define SAMPLE_RATE_HZ 100
#define SAMPLE_DURATION_MS 1000
#define NUM_SAMPLES (SAMPLE_RATE_HZ * SAMPLE_DURATION_MS / 1000)

void collect_training_data(void) {
    float accel_data[NUM_SAMPLES][3];  // x, y, z
    uint32_t timestamp;

    printf("Starting data collection...\n");

    for (int i = 0; i < NUM_SAMPLES; i++) {
        // 读取加速度计
        sensor_read_accelerometer(&accel_data[i][0],
                                   &accel_data[i][1],
                                   &accel_data[i][2]);

        // 添加时间戳
        timestamp = get_timestamp_ms();

        // 存储或传输数据
        log_data(timestamp, accel_data[i]);

        delay_ms(1000 / SAMPLE_RATE_HZ);
    }

    printf("Collected %d samples\n", NUM_SAMPLES);
}
